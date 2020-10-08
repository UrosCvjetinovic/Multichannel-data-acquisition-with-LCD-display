/**
 * @file task2.c
 * @brief Task 2
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * Task niskog prioriteta koji debaunsira tastere S1, S2, S3 i S4
 * Po detektovanju tastera obavestava se Task 1 putem direktne notifikacije taska
 */

/* Standard includes. */
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware includes */
#include "msp430.h"

/* Task includes */
#include "task1.h"
#include "task2.h"
#include "common.h"

/** mp task priority */
#define mainLP_TASK_PRIO        ( 2 )

TaskHandle_t xTask2Handler = NULL;

/** Task debounce binary semaphores */
SemaphoreHandle_t xTask2Debounce;

/**
 * @brief Implementacija 
 * @param pvParameters - ne koristi se
 *
 * 
 */
static void prvTask2 ( void *pvParameters )
{
    static uint8_t ucLastState1 = 0, ucState1 = 0;
    static uint8_t ucLastState2 = 0, ucState2 = 0;
    static uint8_t ucLastState3 = 0, ucState3 = 0;
    static uint8_t ucLastState4 = 0, ucState4 = 0;
    for (;;)
    {
            xSemaphoreTake( xTask2Debounce, portMAX_DELAY );
            P4OUT ^= BIT5;
            /* read S1 */
            ucState1 = P2IN & BIT4;
            if( ( ucState1 == 0 ) && ( ucLastState1 == BIT4 ) )
            {
                /* set event bit */
                xTaskNotify( xTask1Handler, mainS1_Event, eSetBits );
            }
            /* save state */
            ucLastState1 = ucState1;

            /* read S2 */
            ucState2 = P2IN & BIT5;
            if( ( ucState2 == 0 ) && ( ucLastState2 == BIT5 ) )
            {
                /* set event bit */
                xTaskNotify( xTask1Handler, mainS2_Event, eSetBits );
            }
            /* save state */
            ucLastState2 = ucState2;

            /* read S3 */
            ucState3 = P2IN & BIT6;
            if( ( ucState3 == 0 ) && ( ucLastState3 == BIT6 ) )
            {
                /* set event bit */
                xTaskNotify( xTask1Handler, mainS3_Event, eSetBits );
            }
            /* save state */
            ucLastState3 = ucState3;

            /* read S4 */
            ucState4 = P2IN & BIT7;
            if( ( ucState4 == 0 ) && ( ucLastState4 == BIT7 ) )
            {
                /* set event bit */
                xTaskNotify( xTask1Handler, mainS4_Event, eSetBits );
            }
            /* save state */
            ucLastState4 = ucState4;
    }
    vTaskDelete( NULL ); //null suspenzuje pozivanje taska. nece nikad uci trebalo bi
}

/**
 * @brief Inicijalizacija Taska 2
 *
 * Funkcija koja inicijalizuje Task 2
 */
void vTask2Init(void)
{
	xTaskCreate( prvTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, mainLP_TASK_PRIO, &xTask2Handler );

	xTask2Debounce = xSemaphoreCreateBinary();
}

