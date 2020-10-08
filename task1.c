/**
 * @file task1.c
 * @brief Task 1
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * Task visokog prioriteta koji ceka na direktnu notifikaciju setovanu od strane 
 * drugog taska ili prekidne rutine AD konvertora
 * Poruka sadrzi kanal sa kojeg je izvrsena konverzija i 9 donjih bita rezultata konverzije.
 * Po prijemu notifikacije od prekidne rutine AD konvertora, memorise vrednost i 
 * prosledjuje vrednost aktivnog kanala tasku 3
 * Po prijemu notifikacije od strane taska 2, menja se aktivan kanal
 */

/* Standard includes. */
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware includes */
#include "msp430.h"

/* Task includes */
#include "adc_task.h"
#include "task1.h"
#include "task2.h"
#include "common.h"

#define DISP_DATA_QUEUE_LEN   ( 2 )
/** hp task priority */
#define mainHP_TASK_PRIO        ( 4 )

QueueHandle_t xDisplayQueue = NULL;

TaskHandle_t xTask1Handler = NULL;
/**
 * @brief Implementacija 
 * @param pvParameters - ne koristi se
 *
 * 
 */
static void prvTask1 ( void *pvParameters )
{
    uint32_t ulDataToSend;                 // array of values to be sent
    ADCMsg_t ucLast1;
    ADCMsg_t ucLast2;
    ADCMsg_t ucLast3;
    ADCMsg_t ucLast4;                       // results of conversations for all 4 channels
    uint8_t ucSelect = 1;
    uint8_t adc_conv, i;

    uint32_t ulBits = 0;		// bits in event group
    ADCMsg_t xData;


    for(;;)
    {
        P4OUT ^= BIT4;
        /* wait for event */
        xTaskNotifyWait( 0,                 /* don't clear bits on entry */
                         0xffffffff,        /* clear bits on exit */
                         &ulBits,           /* where to store notification value before clear */
                         portMAX_DELAY      /* block until available */
                       );
        adc_conv = (ulBits & (mainADC_CH0_Event | mainADC_CH1_Event | mainADC_CH2_Event | mainADC_CH3_Event )) >> 6;
        i = 0;
        while(adc_conv) { i++; adc_conv>>= 1;}
        for (; i > 0;  i--)
        {
            xQueueReceive( xADCQueue, &xData, portMAX_DELAY );
            switch(xData.eADCNr)
            {
                case 0:
                    ucLast1 = xData;          //place the result in the variable
                    ulDataToSend = ucLast1.ucADCData;
                    if(ucSelect == 1)
                        xQueueOverwrite(xDisplayQueue, &ulDataToSend);
                    break;
                case 1:
                    ucLast2 = xData;
                    ulDataToSend = ucLast2.ucADCData;
                    if(ucSelect == 2)
                        xQueueOverwrite(xDisplayQueue, &ulDataToSend);
                    break;
                case 2:
                    ucLast3 = xData;
                    ulDataToSend = ucLast3.ucADCData;
                    if(ucSelect == 3)
                        xQueueOverwrite(xDisplayQueue, &ulDataToSend);
                    break;
                case 3:
                    ucLast4 = xData;
                    ulDataToSend = ucLast4.ucADCData;
                    if(ucSelect == 4)
                        xQueueOverwrite(xDisplayQueue, &ulDataToSend);
                    break;
                default:
                    break;
            }
        }
        if( ( ulBits & mainS4_Event ) != 0 )
        {
            ucSelect = 4;
        }
        if( ( ulBits & mainS1_Event ) != 0 )
        {
            ucSelect = 1;
        }
        if( ( ulBits & mainS2_Event ) != 0 )
        {
            ucSelect = 2;
        }
        if( ( ulBits & mainS3_Event ) != 0 )
        {
            ucSelect = 3;
        }
        ulBits = 0;
    }
    vTaskDelete( NULL ); //null suspenzuje pozivanje taska. nece nikad uci trebalo bi
}

/**
 * @brief Inicijalizacija Taska 1
 *
 * Funkcija koja inicijalizuje Task 1 i kreira xDisplayQueue
 */
void vTask1Init(void)
{
	xTaskCreate( prvTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, mainHP_TASK_PRIO, &xTask1Handler );
    xDisplayQueue = xQueueCreate(1, sizeof(uint16_t));                 //Queue for communication between task1 and task3
}

