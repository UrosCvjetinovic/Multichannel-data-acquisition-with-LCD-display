/**
 * @file task2.h
 * @brief Deklaracje taska 2
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 */

#ifndef TASK2_H_
#define TASK2_H_

#include "semphr.h"

/** S1_Event bit */
#define mainS1_Event            ( BIT2 )
/** S2_Event bit */
#define mainS2_Event            ( BIT3 )
/** S3_Event bit */
#define mainS3_Event            ( BIT4 )
/** S4_Event bit */
#define mainS4_Event            ( BIT5 )


/* Task Handler, for timer to notify task 2 */
extern TaskHandle_t xTask2Handler;
/**
 * @brief Inicijalizacija Taska 2
 *
 * Funkcija koja inicijalizuje Task 2
 */
extern void vTask2Init(void);

/** Task debounce binary semaphores */
extern SemaphoreHandle_t xTask2Debounce;

#endif /*TASK2_H_ */
