/**
 * @file adc_task.h
 * @brief Deklaracija ADC taska (TaskTimer)
 * @author Uros Cvjetinovic 
 * @contact cu160093d@student.etf.bg.ac.rs, ucvjetinovic@gmail.com
 * @date 2020
 */

#ifndef ADC_TASK_H_
#define ADC_TASK_H_

/**
 * Tip za opis rednog broja kanala
 */
typedef enum
{
    CH0,/**< kanal 0 */
    CH1,/**< kanal 1 */
    CH2,/**< kanal 2 */
    CH3 /**< kanal 3 */
} Channel_t;

/**
 * Struktura koja sluzi za slanje ADC poruke.
 */
typedef struct
{
	Channel_t eADCNr;	/**< Redni broj kanala sa kog je ocitana vrednost */
	uint16_t ucADCData; /**< 12-bitni rezultat konverzije */
} ADCMsg_t;

/** S2_Event bit */
#define mainADC_CH0_Event            ( BIT6 )
/** S2_Event bit */
#define mainADC_CH1_Event            ( BIT7 )
/** S2_Event bit */
#define mainADC_CH2_Event            ( BIT8 )
/** S2_Event bit */
#define mainADC_CH3_Event            ( BIT9 )

/**
 * @brief Inicijalizacija ADC taska
 *
 * Funkcija koja inicijalizuje ADC task i kreira odgovarajuci red sa porukama.
 */
extern void vTaskTimerInit(void);

/**
 * @brief Inicijalizacija AD konvertora
 *
 * Funkcija koja konfigurise hardver AD konvertora.
 * Multipleksira odgovarajuce pinove tako da se koriste za AD konvertor.
 * Konfigurise AD konvertor da radi u rezimu Repeat Sequence of channels.
 * Koristi memorijske lokacije 0 do 3 za smestanje rezultata konverzije.
 */
extern void vADCInitHardware(void);

/**
 * Red sa porukama u koji se u prekidnoj rutini smestaju rezultati konverzije
 */
extern QueueHandle_t xADCQueue;

#endif /* ADC_TASK_H_ */
