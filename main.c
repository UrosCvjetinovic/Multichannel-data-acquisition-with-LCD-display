/**
 * @file main.c
 * @brief Main for project cu160093
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * Postoje cetiri taska.
 * xTaskTimer, najviseg prioriteta, startuje AD konverziju na svakih 1000ms
 * xTask1 pamti odabirke i od kog kanala su, zatim prosledjuje vrednost 
 * jednog kanala tasku 3 zavisno od setovanog aktivnog kanala preko taska 2
 * xTask3 je sledeci po prioritetu. On racuna razliku uzastopnih poslatih 
 * vrednosti i ispisuje na led displej
 * xTask2, najnizeg prioriteta, debaunsira tastere i notifikuje Task 1 ako 
 * dodje do detekcije
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware includes. */
#include "msp430.h"
#include "hal_ETF5438A.h"

#include "adc_task.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "timerled.h"
#include "timerdebounce.h"

static void prvSetupHardware( void );

/**
 * @brief Glavna funkcija
 *
 * Funkcija koja inicijalizuje taskove i objekte RTOS i startuje Scheduler
 */
void main( void )
{
    /* Create tasks and timer */
    vTaskTimerInit();
    vTask1Init();
    vTask2Init();
    vTask3Init();
    vTimerLEDInit();
    vTimerDebounceInit();

    /* Configure peripherals */
    prvSetupHardware();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well then this line will never be reached.  If it is reached
    then it is likely that there was insufficient (FreeRTOS) heap memory space
    to create the idle task.  This may have been trapped by the malloc() failed
    hook function, if one is configured. */
    for( ;; );
}

/**
 * @brief Inicijalizacija hardvera
 *
 * Standardna inicijalizacija hardvera
 */
static void prvSetupHardware( void )
{
    taskDISABLE_INTERRUPTS();

    /* Disable the watchdog. */
    WDTCTL = WDTPW + WDTHOLD;

    /* Configure Clock. Since we aren't using XT1 on the board,
     * configure REFOCLK to source FLL adn ACLK.
     */
    SELECT_FLLREF(SELREF__REFOCLK);
    SELECT_ACLK(SELA__REFOCLK);
    hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

    /* Enable LED display */
    P11DIR |= BIT0 + BIT1;
    P10DIR |= BIT6 + BIT7;
    P6DIR = 0x7F;

    /* Enable buttons */
    P2DIR &= ~(0xF0);
    P4DIR |= 0x78;
    P4OUT = 0x00;
    /* Initialize necessary hardware */
    vADCInitHardware();
}
