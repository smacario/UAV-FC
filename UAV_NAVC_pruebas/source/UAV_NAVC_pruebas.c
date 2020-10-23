#include <MMA8451.h>
#include <stdio.h>

#include "MKL43Z4.h"
#include "init_board.h"
#include "MAG3110.h"
#include "board.h"
#include "UAV_NAVC_pruebas.h"

#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "math.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"


static int16_t imu_readX, imu_readY, imu_readZ;		// Lectura de acelerometro
static int32_t mag_readX, mag_readY, mag_readZ;		// Lectura de magnetometro

int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;
int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;

static float heading_angle_rad = 0.0f;				// Angulo de direccion en rad
static float heading_angle_deg = 0.0f;				// Angulo de dirección en grados


Mag mag;
Imu imu;



int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    BOARD_InitDebugConsole();
    SysTick_Config(SystemCoreClock / 1000U);

    Board_Init();
    Config_Port_Int();


    while(1) {

    	mag.X = mag_readX - X_Mag_Offset;
    	mag.Y = mag_readY - Y_Mag_Offset;
    	mag.Z = mag_readZ - Z_Mag_Offset;

    	imu.X = imu_readX - X_Acc_Offset;
    	imu.Y = imu_readY - Y_Acc_Offset;
    	imu.Z = imu_readZ - Z_Acc_Offset;


    	heading_angle_rad = atan2(mag.X, mag.Y);
    	if(heading_angle_rad >= 0) heading_angle_deg = heading_angle_rad * (180.0f / _PI);
    	else heading_angle_deg = (heading_angle_rad + 2.0f * _PI) * (180.0f / _PI);


    	PRINTF("ACC: x=%i, y=%i, z=%i ", imu.X, imu.Y, imu.Z);
    	PRINTF("   Heading: %i \n", (int16_t)heading_angle_deg);
    }
    return 0 ;
}


void Config_Port_Int(void)
{
	const port_pin_config_t port_int1_config = {
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
	const gpio_pin_config_t gpio_int1_config = {
		.pinDirection = kGPIO_DigitalInput,
		.outputLogic = 0U
	};

	PORT_SetPinConfig(ACC_INT1_PORT, ACC_INT1_PIN, &port_int1_config);
	PORT_SetPinConfig(MAG_INT1_PORT, MAG_INT1_PIN, &port_int1_config);
	GPIO_PinInit(ACC_INT1_GPIO, ACC_INT1_PIN, &gpio_int1_config);
	GPIO_PinInit(MAG_INT1_GPIO, MAG_INT1_PIN, &gpio_int1_config);

	/* Interrupt polarity active high, or active low. Default value: 0.
	   0: Active low; 1: Active high. VER REGISTRO CTRL_REG3 */
	PORT_SetPinInterruptConfig(ACC_INT1_PORT, ACC_INT1_PIN, kPORT_InterruptLogicZero);
	PORT_SetPinInterruptConfig(MAG_INT1_PORT, MAG_INT1_PIN, kPORT_InterruptRisingEdge);

	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	NVIC_SetPriority(PORTC_PORTD_IRQn, 0);
}


void PORTC_PORTD_IRQHandler(void)
{
    int16_t readG, readM;

    ACC_INT_SOURCE_t 	intSource;
    ACC_STATUS_t 		acc_status;
    MAG_STATUS_t 		mag_status;

    // Leo flag de interrupcion
    uint32_t PORTC_int = PORT_GetPinsInterruptFlags(PORTC);
    uint32_t PORTD_int = PORT_GetPinsInterruptFlags(PORTD);

    mag_status.data = MAG3110_read_reg(MAG_STATUS_ADDRESS);


    if(PORTC_int && (1 << 5)){

        intSource.data  = mma8451_read_reg(ACC_INT_SOURCE_ADDRESS);
        //mag_status.data = MAG3110_read_reg(MAG_STATUS_ADDRESS);

    	if (intSource.SRC_DRDY){
			acc_status.data = mma8451_read_reg(ACC_STATUS_ADDRESS);

			if (acc_status.XDR){
				readG   = (int16_t)mma8451_read_reg(0x01)<<8;
				readG  |= mma8451_read_reg(0x02);
				imu_readX = readG >> 2;

				if(mag_status.XDR || mag_status.XOW){
					readM   = (int16_t)MAG3110_read_reg(0x01)<<8;
					readM  |= MAG3110_read_reg(0x02);
					mag_readX = readM >> 2;
				}

			}

			if (acc_status.YDR){
				readG   = (int16_t)mma8451_read_reg(0x03)<<8;
				readG  |= mma8451_read_reg(0x04);
				imu_readY = readG >> 2;

				if(mag_status.YDR || mag_status.YOW){
					readM   = (int16_t)MAG3110_read_reg(0x03)<<8;
					readM  |= MAG3110_read_reg(0x04);
					mag_readY = readM >> 2;
				}

			}

			if (acc_status.ZDR){
				readG   = (int16_t)mma8451_read_reg(0x05)<<8;
				readG  |= mma8451_read_reg(0x06);
				imu_readZ = readG >> 2;

				if(mag_status.ZDR || mag_status.ZOW){
					readM   = (int16_t)MAG3110_read_reg(0x05)<<8;
					readM  |= MAG3110_read_reg(0x06);
					mag_readZ = readM >> 2;
				}

			}
		}

    	PORT_ClearPinsInterruptFlags(ACC_INT1_PORT, 1<<ACC_INT1_PIN);
    }

    if(PORTD_int && (1 << 1)){

    	if (mag_status.XDR | mag_status.XOW){
			readM   = (int16_t)MAG3110_read_reg(0x01)<<8;
			readM  |= MAG3110_read_reg(0x02);
			mag_readX = readM >> 2;
		}

		if (mag_status.YDR | mag_status.YOW){
			readM   = (int16_t)MAG3110_read_reg(0x03)<<8;
			readM  |= MAG3110_read_reg(0x04);
			mag_readY = readM >> 2;
		}

		if (mag_status.ZDR | mag_status.ZOW){
			readM   = (int16_t)MAG3110_read_reg(0x05)<<8;
			readM  |= MAG3110_read_reg(0x06);
			mag_readZ = readM >> 2;
		}

    	PORT_ClearPinsInterruptFlags(MAG_INT1_PORT, 1<<MAG_INT1_PIN);
    }

}










