/**
 * @file task3.c
 * @brief Task 3
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * Task srednjeg prioriteta koji ceka na redu sa porukama koju prosledjuje Task 1 i 
 * ispisuje razliku uzastopnih na LED displej.
 * Poruka sadrzi rezultat konverzije. Po prijemu poruke vrednost se oduzima od prethodne 
 * preuzete vrednosti, i poziva funkcije koje ispisuju razliku na LED displej.
 * Nova preuzeta vrednost se pamti umesto prethodne. Pri pokretanju sistema prethodna je 0.
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
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "common.h"

volatile int ucNumber1;
volatile int ucNumber2;
volatile int ucNumber3;
volatile int ucNumber4;

/** mp task priority */
#define mainMP_TASK_PRIO        ( 3 )

/**
 * @brief Function used to separate digits from package
 * @param ulData - package for separation
 *
 */
void GetDigits(uint16_t ulData)
{
       volatile int ulBuffer;
       ulBuffer= ulData;

       ucNumber4 =  ulBuffer / 1000;
       ucNumber3 = (ulBuffer / 100) % 10;
       ucNumber2 = (ulBuffer / 10)  % 10;
       ucNumber1 =  ulBuffer        % 10;
       if (ulBuffer < 0)
       {
           if ( (ucNumber4 < 0) || (ucNumber4 > 9) )
           {
               ucNumber4 = 10;
               ucNumber3 = 10;
               ucNumber2 = 10;
               ucNumber1 = 10;
           }
           else
           {
               ucNumber4 = 11;
               ucNumber3 = -ucNumber3;
               ucNumber2 = -ucNumber2;
               ucNumber1 = -ucNumber1;
           }
       }
}

/**
 * @brief Implementacija 
 * @param pvParameters - ne koristi se
 *
 * 
 */
static void prvTask3 ( void *pvParameters )
{

    uint16_t ucLast = 0;
    uint16_t ulBuff;                        // buffer for receiving values from xTask1

    for (;;)
    {
        /* if a mailbox filled then display value */
        xQueueReceive(xDisplayQueue, (void* ) &ulBuff, portMAX_DELAY);
        P4OUT ^= BIT6;
        GetDigits(ulBuff - ucLast);
        ucLast = ulBuff;
    }
    vTaskDelete( NULL ); //null suspenzuje pozivanje taska. nece nikad uci trebalo bi
}

/**
 * @brief Inicijalizacija Taska 3
 *
 * Funkcija koja inicijalizuje Task 3
 */
void vTask3Init(void)
{
	xTaskCreate( prvTask3, "Task3", configMINIMAL_STACK_SIZE, NULL, mainMP_TASK_PRIO, NULL );
}

