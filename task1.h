/**
 * @file task1.h
 * @brief Deklaracija taska 1
 * @author Uros Cvjetinovic
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 */

#ifndef TASK1_H_
#define TASK1_H_

/**
 * @brief Inicijalizacija Taska 1
 *
 * Funkcija koja inicijalizuje Task 1
 */
extern void vTask1Init(void);
extern QueueHandle_t xDisplayQueue;

extern TaskHandle_t xTask1Handler;

#endif /* TASK1_H__H_ */
