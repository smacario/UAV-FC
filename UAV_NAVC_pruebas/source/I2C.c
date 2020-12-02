#include <board.h>

#include "fsl_port.h"
#include "fsl_i2c.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"

#include "pin_mux.h"


#define I2C_RELEASE_SDA_PORT 		PORTE
#define I2C_RELEASE_SCL_PORT 		PORTE
#define I2C_RELEASE_SDA_GPIO 		GPIOE
#define I2C_RELEASE_SDA_PIN 		25U
#define I2C_RELEASE_SCL_GPIO 		GPIOE
#define I2C_RELEASE_SCL_PIN 		24U
#define I2C_RELEASE_BUS_COUNT 		100U

#define I2C_BAUDRATE 				100000U


static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++)
    {
        __NOP();
    }
}

/*
 * Esta función enviá configura los pines I2C como GPIO para generar una secuencia
 * de start, luego envía 9 pulsos en SCL manteniendo SDA en alto para que ningún
 * dispositivo I2C se identifique y finalmente envía un STOP. Esto se hace por
 * si el programa se reinicia en medio de una transferencia I2C haciendo que los
 * dispositivos no completen la transferencia.
 */
static void I2C_releaseBus(void)
{
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    CLOCK_EnableClock(kCLOCK_PortE);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++)
    {
        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

static void I2C_configPins(void)
{
	const port_pin_config_t porte24_pin31_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Slow slew rate is configured */
													kPORT_SlowSlewRate,
													/* Passive filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as I2C0_SCL */
													kPORT_MuxAlt5};
	/* PORTE24 (pin 31) is configured as I2C0_SCL */
	PORT_SetPinConfig(PORTE, 24U, &porte24_pin31_config);

	const port_pin_config_t porte25_pin32_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Slow slew rate is configured */
													kPORT_SlowSlewRate,
													/* Passive filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as I2C0_SDA */
													kPORT_MuxAlt5};
	/* PORTE25 (pin 32) is configured as I2C0_SDA */
	PORT_SetPinConfig(PORTE, 25U, &porte25_pin32_config);

}


void I2C_init(void)
{
	i2c_master_config_t masterConfig;
	uint32_t sourceClock;

	I2C_releaseBus();

	I2C_configPins();

	I2C_MasterGetDefaultConfig(&masterConfig);

	masterConfig.baudRate_Bps = I2C_BAUDRATE;

	sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

	I2C_MasterInit(I2C0, &masterConfig, sourceClock);
}


