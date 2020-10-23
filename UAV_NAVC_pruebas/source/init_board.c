/*
 * SD2_board.c
 *
 *  Created on: 12 Apr 2020
 *      Author: Santiago
 */


/*==================[inclusions]=============================================*/
#include <init_board.h>
#include <MMA8451.h>
#include "I2C.h"
#include "MAG3110.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "pin_mux.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
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

static bool ledStatus[BOARD_LED_ID_TOTAL];

static const port_mux_t pinMuxLeds[BOARD_LED_ID_TOTAL] =
{
	kPORT_MuxAlt3,
	kPORT_MuxAlt4,
};

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/


/*==================[external functions definition]==========================*/


void Board_Init(void)
{

	int32_t i;
	gpio_pin_config_t gpio_led_config =
	{
		.outputLogic = 1,
		.pinDirection = kGPIO_DigitalOutput,
	};
	gpio_pin_config_t gpio_sw_config = {
		.pinDirection = kGPIO_DigitalInput,
		.outputLogic = 0U
	};

	const port_pin_config_t port_led_config = {
		/* Internal pull-up/down resistor is disabled */
		.pullSelect = kPORT_PullDisable,
		/* Slow slew rate is configured */
		.slewRate = kPORT_SlowSlewRate,
		/* Passive filter is disabled */
		.passiveFilterEnable = kPORT_PassiveFilterDisable,
		/* Low drive strength is configured */
		.driveStrength = kPORT_LowDriveStrength,
		/* Pin is configured as PTC3 */
		.mux = kPORT_MuxAsGpio,
	};

	const port_pin_config_t port_sw_config = {
		/* Internal pull-up resistor is enabled */
		.pullSelect = kPORT_PullUp,
		/* Fast slew rate is configured */
		.slewRate = kPORT_FastSlewRate,
		/* Passive filter is disabled */
		.passiveFilterEnable = kPORT_PassiveFilterDisable,
		/* Low drive strength is configured */
		.driveStrength = kPORT_LowDriveStrength,
		/* Pin is configured as PTC3 */
		.mux = kPORT_MuxAsGpio,
	};

	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);
	CLOCK_EnableClock(kCLOCK_PortE);

	/* inicialización de leds */
	for (i = 0 ; i < BOARD_LED_ID_TOTAL ; i++)
	{
		PORT_SetPinConfig(board_gpioLeds[i].port, board_gpioLeds[i].pin, &port_led_config);
		GPIO_PinInit(board_gpioLeds[i].gpio, board_gpioLeds[i].pin, &gpio_led_config);
	}

	/* inicialización de SWs */
	for (i = 0 ; i < BOARD_SW_ID_TOTAL ; i++)
	{
		PORT_SetPinConfig(board_gpioSw[i].port, board_gpioSw[i].pin, &port_sw_config);
		GPIO_PinInit(board_gpioSw[i].gpio, board_gpioSw[i].pin, &gpio_sw_config);
	}

	/* =========== I2C =================== */

	I2C_init();
	acc_init();
	MAG3310_init();

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

void board_rs485_setRE(board_reMsg_enum msg) {

	switch(msg){
		case BOARD_RE_MSG_OFF:
			GPIO_PortClear(board_gpioLeds[BOARD_RE].gpio, 1<<board_gpioLeds[BOARD_RE].pin);
		break;

		case BOARD_RE_MSG_ON:
			GPIO_PortSet(board_gpioLeds[BOARD_RE].gpio, 1<<board_gpioLeds[BOARD_RE].pin);
		break;
	}
}

void board_rs485_setDE(board_deMsg_enum msg) {

	switch(msg){
		case BOARD_DE_MSG_OFF:
			GPIO_PortClear(board_gpioLeds[BOARD_DE].gpio, 1<<board_gpioLeds[BOARD_DE].pin);
		break;

		case BOARD_DE_MSG_ON:
			GPIO_PortSet(board_gpioLeds[BOARD_DE].gpio, 1<<board_gpioLeds[BOARD_DE].pin);
		break;
	}
}


bool board_getSw(board_swId_enum id)
{
    // return !GPIO_ReadPinInput(board_gpioSw[id].gpio, board_gpioSw[id].pin);		//	ORIGINAL (NO EXISTE GPIO_ReadPinInput() en libreria de fabricante)
	return !GPIO_PinRead(board_gpioSw[id].gpio, board_gpioSw[id].pin);				//  MODIFICACION PARA KL43Z
}

