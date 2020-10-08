/**
 * @file common.h
 * @brief Zajednicke deklaracije
 * @author Uros Cvjetinovic 
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 */

#ifndef COMMON_H_
#define COMMON_H_

/** delay used for task synchronization */
#define mainTASK_SYNC_DELAY     ( pdMS_TO_TICKS( 20 ) )     // ( ( ( TickType_t ) 20 ) / portTICK_PERIOD_MS )

#endif /* COMMON_H_ */
