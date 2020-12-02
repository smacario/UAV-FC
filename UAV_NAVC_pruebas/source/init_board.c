#include <init_board.h>
#include <QMC5883L.h>
#include <BMI160.h>

#include "I2C.h"
#include "pin_mux.h"

#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "fsl_lpuart.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"


static const board_gpioInfo_type board_gpioLeds[] =
{
    {PORTE, GPIOE, 31},     /* LED ROJO */
    {PORTD, GPIOD, 5},      /* LED VERDE */
	{PORTD, GPIOD, 6},		/* DE */
    {PORTD, GPIOD, 4},		/* RE */

};


static const board_gpioInfo_type board_gpioSw[] =
{
    {PORTA, GPIOA, 4},      /* SW1 */
    {PORTC, GPIOC, 3},      /* SW3 */
};


void Board_Init(void){

	I2C_init();					// Inicializacion modulo I2C

	BMI160_init();				// Inicializacion de IMU
	QMC5883L_init();			// Inicializacion de Magnetometro

}


void board_setLed(board_ledId_enum id, board_ledMsg_enum msg)
{
    switch (msg)
    {
        case BOARD_LED_MSG_OFF:
        	GPIO_PortSet(board_gpioLeds[id].gpio, 1<<board_gpioLeds[id].pin);
            break;

        case BOARD_LED_MSG_ON:
        	GPIO_PortClear(board_gpioLeds[id].gpio, 1<<board_gpioLeds[id].pin);
            break;

        case BOARD_LED_MSG_TOGGLE:
        	GPIO_PortToggle(board_gpioLeds[id].gpio, 1<<board_gpioLeds[id].pin);
            break;

        default:
            break;
    }
}



bool board_getSw(board_swId_enum id)
{
    // return !GPIO_ReadPinInput(board_gpioSw[id].gpio, board_gpioSw[id].pin);		//	ORIGINAL (NO EXISTE GPIO_ReadPinInput() en libreria de fabricante)
	return !GPIO_PinRead(board_gpioSw[id].gpio, board_gpioSw[id].pin);				//  MODIFICACION PARA KL43Z
}

