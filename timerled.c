/**
 * @file timerled.c
 * @brief Implementacija timera LED 
 * @author Uros Cvjetinovic 
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 *
 * ADC task startuje konverziju svaki put kada mu stigne notifikacija.
 * U prekidnoj rutini se rezultat konverzije smesta u red sa porukama.
 */
 
/* Standard includes. */
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Hardware includes */
#include "msp430.h"

/* Timer include */
#include "common.h"
#include "timerled.h"
#include "task3.h"

/** Timer period for display multiplex */
#define mainTIMERLED_PERIOD     ( pdMS_TO_TICKS( 90 ) )

/** Timer handler */
TimerHandle_t xTimerLED = NULL;

/* Look up table for display codes */
const unsigned int segtab[] = {
        0x7E,
        0x30,
        0x6D,
        0x79,
        0x33,
        0x5b,
        0x5f,
        0x70,
        0x7f,
        0x7b,
        0x4f,
        0x01
};

/**
 * @name vTimerLEDCallback
 * @brief Timer callback function for LED display
 */
static void vTimerLEDCallback( TimerHandle_t xTimer )
{
    static uint8_t ucNumber = 0;
    switch(ucNumber)
       {
       case 0:
           P11OUT |= BIT1;
           P6OUT = segtab[ucNumber1];
           P10OUT &=~ BIT6;
           ucNumber = 1;
           break;
       case 1:
           P10OUT |= BIT6;
           P6OUT = segtab[ucNumber2];
           P10OUT &=~ BIT7;
           ucNumber = 2;
           break;
       case 2:
           P10OUT |= BIT7;
           P6OUT = segtab[ucNumber3];
           P11OUT &=~ BIT0;
           ucNumber = 3;
           break;
       case 3:
           P11OUT |= BIT0;
           P6OUT = segtab[ucNumber4];
           P11OUT &=~ BIT1;
           ucNumber = 0;
           break;
       }
}

/**
 * @brief Inicijalizacija Taska 3
 *
 * Funkcija koja inicijalizuje Task 3
 */
void vTimerLEDInit(void)
{
	/* Create software timers */
    xTimerLED = xTimerCreate( "TimerLED",mainTIMERLED_PERIOD,pdTRUE,NULL,vTimerLEDCallback);

    /* Start timers */
    xTimerStart( xTimerLED, 0 );
}
