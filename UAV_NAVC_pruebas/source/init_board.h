/*
 * SD2_board.h
 *
 *  Created on: 12 Apr 2020
 *      Author: Santiago
 */

#ifndef INIT_BOARD_H_
#define INIT_BOARD_H_

#include "MKL43Z4.h"
#include "stdbool.h"

/*==================[macros]=================================================*/

typedef enum
{
    BOARD_LED_ID_ROJO = 0,
    BOARD_LED_ID_VERDE,
	BOARD_DE,
	BOARD_RE,
	BOARD_LED_ID_TOTAL
}board_ledId_enum;

typedef enum
{
    BOARD_SW_ID_1 = 0,
    BOARD_SW_ID_3,
    BOARD_SW_ID_TOTAL
}board_swId_enum;

typedef enum
{
    BOARD_LED_MSG_OFF = 0,
    BOARD_LED_MSG_ON,
    BOARD_LED_MSG_TOGGLE
}board_ledMsg_enum;

typedef enum
{
	BOARD_DE_MSG_OFF = 0,
	BOARD_DE_MSG_ON,
}board_deMsg_enum;

typedef enum
{
	BOARD_RE_MSG_OFF = 0,
	BOARD_RE_MSG_ON,
}board_reMsg_enum;

typedef enum
{
    DR_800hz = 0b000,
    DR_400hz = 0b001,
    DR_200hz = 0b010,
    DR_100hz = 0b011,
    DR_50hz = 0b100,
    DR_12p5hz = 0b101,
    DR_6p25hz = 0b110,
    DR_1p56hz = 0b111,
}DR_enum;

typedef struct
{
    PORT_Type *port;
    GPIO_Type *gpio;
    uint32_t pin;
}board_gpioInfo_type;

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions definition]==========================*/

/** \brief inicializaci�n del hardware
 **
 **/
void board_init(void);

/** \brief setea estado del led indicado
 **
 **/
void board_setLed(board_ledId_enum id, board_ledMsg_enum msg);
void board_rs485_setRE(board_reMsg_enum msg);
void board_rs485_setDE(board_deMsg_enum msg);

/** \brief Devuelve estado del pulsador indicado
 **
 ** \return true: si el pulsdor est� apretado
 **         false: si el pulsador no est� apretado
 **/
bool board_getSw(board_swId_enum id);

void board_setLedBrightness(board_ledId_enum id, uint8_t brightness);


#endif /* INIT_BOARD_H_ */
