/**
 * @file adc_task.c
 * @brief Implementacija ADC taska (TaskTimer) i prekidne rutine
 * @author Uros Cvjetinovic 
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * ADC task startuje konverziju svaki put kada mu stigne notifikacija.
 * U prekidnoj rutini se rezultat konverzije smesta u red sa porukama.
 */


/* Standard includes. */
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware includes. */
#include "msp430.h"

/* Task includes. */
#include "adc_task.h"
#include "task1.h"
/**
 * Prioritet ADC taska. Najvisi prioritet.
 */
#define adcTASK_PRIO			( 5 )

/**
 * Duzina reda sa porukama.
 */
#define adcADC_DATA_QUEUE_LEN	( 16 )

/**
 * Makro za startovanje konverzije
 */
#define adcSTART_CONV			do { ADC12CTL0 |= ADC12SC; } while( 0 )

/**
 * Handle ADC taska, za direktnu notifikaciju
 */
TaskHandle_t TaskTimerHandler;

/**
 * Red sa porukama u koji se u prekidnoj rutini smestaju rezultati konverzije
 */
QueueHandle_t xADCQueue;

/** Timer period for display multiplex */
#define mainAQUISITION_PERIOD     ( pdMS_TO_TICKS( 1000 ) )
/**
 * @brief Implementacija ADC taska
 * @param pvParameters - ne koristi se
 *
 * ADC task startuje konverziju kada mu stigne notifikacija
 */
static void prvTaskTimer( void *pvParameters )
{

    TickType_t xLastWakeTime;

    //Inicijalizuje xLastWakeTime varijablu sa trenutnom vrednoscu

    xLastWakeTime = xTaskGetTickCount();

    for(;;) {
        P4OUT ^= BIT3;
        adcSTART_CONV;
        vTaskDelayUntil( &xLastWakeTime, mainAQUISITION_PERIOD);
    }

    vTaskDelete( NULL ); //null suspenzuje pozivanje taska. nece nikad uci trebalo bi
}

/**
 * @brief Inicijalizacija ADC taska
 *
 * Funkcija koja inicijalizuje ADC task i kreira odgovarajuci red sa porukama.
 */
void vTaskTimerInit(void)
{
	xTaskCreate( prvTaskTimer, "TaskTimer", configMINIMAL_STACK_SIZE, NULL, adcTASK_PRIO, &TaskTimerHandler );
	xADCQueue = xQueueCreate( adcADC_DATA_QUEUE_LEN, sizeof( ADCMsg_t ) );
}

/**
 * @brief Inicijalizacija AD konvertora
 *
 * Funkcija koja konfigurise hardver AD konvertora.
 * Multipleksira odgovarajuce pinove tako da se koriste za AD konvertor.
 * Konfigurise AD konvertor da radi u rezimu Repeat Sequence of channels.
 * Koristi memorijske lokacije 0 do 3 za smestanje rezultata konverzije.
 */
void vADCInitHardware(void)
{
	/* multipleksiranje pinova */
	P7SEL |= BIT6 + BIT7;
	P5SEL |= BIT0 + BIT1;

    /* podesavanje AD konvertora */
    ADC12CTL0 = ADC12ON | ADC12MSC;     /* koristi MSC */
    ADC12CTL1 = ADC12SHS_0 | ADC12CONSEQ_1 | ADC12SHP;  /* repeat-sequence, SC startuje */
    ADC12MCTL0 = ADC12INCH_14;
    ADC12MCTL1 = ADC12INCH_15;
    ADC12MCTL2 = ADC12INCH_8;
    ADC12MCTL3 = ADC12INCH_9 | ADC12EOS;    /* MEM3 je kraj sekvence */
    ADC12IE |= ADC12IE0 | ADC12IE1 | ADC12IE2 | ADC12IE3;   /* dozvoli sve prekide */
    ADC12CTL0 |= ADC12ENC;      /* dozvoli konverziju */
}

/**
 * @brief Prekidna rutina ADC
 *
 * Unutar prekidne rutine se rezultat konverzije upisuje u red sa porukama.
 */
void __attribute__ ( ( interrupt( ADC12_VECTOR ) ) ) vADCISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch( ADC12IV )
    {
        case  6:                    /* Vector  6: ADC12IFG0 */
        {
            ADCMsg_t xMsg = { CH0, ADC12MEM0 >> 3};
            xQueueSendToBackFromISR( xADCQueue, &xMsg, &xHigherPriorityTaskWoken );
            /* set event bit */
            xTaskNotifyFromISR( xTask1Handler, mainADC_CH0_Event, eSetBits, &xHigherPriorityTaskWoken );
            break;
        }
        case  8:                    /* Vector  8: ADC12IFG1 */
        {
            ADCMsg_t xMsg = { CH1, ADC12MEM1 >> 3};
            xQueueSendToBackFromISR( xADCQueue, &xMsg, &xHigherPriorityTaskWoken );
            /* set event bit */
            xTaskNotifyFromISR( xTask1Handler, mainADC_CH1_Event, eSetBits, &xHigherPriorityTaskWoken );
            break;
        }
        case 10:                    /* Vector 10: ADC12IFG2 */
        {
            ADCMsg_t xMsg = { CH2, ADC12MEM2 >> 3};
            xQueueSendToBackFromISR( xADCQueue, &xMsg, &xHigherPriorityTaskWoken );
            /* set event bit */
            xTaskNotifyFromISR( xTask1Handler, mainADC_CH2_Event, eSetBits, &xHigherPriorityTaskWoken );
            break;
        }
        case 12:                    /* Vector 12: ADC12IFG3 */
        {
            ADCMsg_t xMsg = { CH3, ADC12MEM3 >> 3};
            xQueueSendToBackFromISR( xADCQueue, &xMsg, &xHigherPriorityTaskWoken );
            /* set event bit */
            xTaskNotifyFromISR( xTask1Handler, mainADC_CH3_Event, eSetBits, &xHigherPriorityTaskWoken );
            break;
        }
        default: break;
    }
}
