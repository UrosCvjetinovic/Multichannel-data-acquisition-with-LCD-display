/**
 * @file timerdebounce.c
 * @brief Implementacija tajmera za debounsiranje
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
#include "semphr.h"

/* Hardware includes */
#include "msp430.h"

/* Timer include */
#include "timerdebounce.h"
#include "task2.h"

/** Timer period for display multiplex */
#define mainTIMERDEBOUNCE_PERIOD     ( pdMS_TO_TICKS( 200 ) )

/** Timer handler */
TimerHandle_t xTimerDebounce = NULL;
/**
 * @name vTimerDebounceCallback
 * @brief Timer callback function for debouncing with task 2
 */
static void vTimerDebounceCallback( TimerHandle_t xTimer )
{
    //P4OUT ^= BIT5;
    /* notify task */
    xSemaphoreGive( xTask2Debounce );
}

/**
 * @brief Inicijalizacija Tajmera za debounsiranje
 *
 * Funkcija koja inicijalizuje tajmer za debounsiranje
 */
void vTimerDebounceInit(void)
{
	    /* Create software timers */
    xTimerDebounce = xTimerCreate( "TimerDebounce",mainTIMERDEBOUNCE_PERIOD,pdTRUE,NULL,vTimerDebounceCallback);

    /* Start timers */
    xTimerStart( xTimerDebounce, 0 );
}
