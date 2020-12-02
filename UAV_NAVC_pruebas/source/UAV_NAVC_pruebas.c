
/*  Proyecto UAV-NAVC  */

#include <stdio.h>
#include "math.h"

#include "MKL43Z4.h"
#include "board.h"
#include "init_board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include <QMC5883L.h>
#include <BMI160.h>
#include <GPS.h>
#include <UAV_NAVC_pruebas.h>

#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_lpuart.h"
#include "fsl_uart.h"


uint8_t RxBuffer[LPUART_RING_BUFFER_SIZE];					// Buffer de Rx para LPUART0
volatile uint16_t txIndex;									// Index de Tx
volatile uint16_t rxIndex; 									// Index de Rx
uint8_t NAVC_DATA_BUFFER[12];								// Buffer de datos de la NAVC

static int16_t acc_readX, acc_readY, acc_readZ;				// Lectura de acelerometro
static int32_t mag_readX, mag_readY, mag_readZ;				// Lectura de magnetometro
static int16_t gyr_readX, gyr_readY, gyr_readZ;				// Lectura de giroscopio

int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;			// Offset automatico magnetometro
int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;			// Offset automatico acelerometro

static float Pitch, Roll, Yaw;								// Angulos con respecto a los ejes X, Y, Z

Mag mag;													// Datos del magnetometro
Imu imu;													// Datos del acelerometro

GPS_Data GPS;												// Datos del GPS


/*  Main  */
int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    BOARD_InitDebugConsole();
    SysTick_Config(SystemCoreClock / 1000U);

    Board_Init();
    Config_Port_Int();

    while(1){


/*
		// Compensacion de datos por calibracion automatica
    	mag.X = mag_readX - X_Mag_Offset;
    	mag.Y = mag_readY - Y_Mag_Offset;
    	mag.Z = mag_readZ - Z_Mag_Offset;
*/
    	imu.X = acc_readX - X_Acc_Offset;
    	imu.Y = acc_readY - Y_Acc_Offset;
    	imu.Z = acc_readZ - Z_Acc_Offset;

    	// Compensacion de datos estaticos
    	mag.X = mag_readX - X_MAG_OFFSET_STATIC;
		mag.Y = mag_readY - Y_MAG_OFFSET_STATIC;
		mag.Z = mag_readZ - Z_MAG_OFFSET_STATIC;

    	Compass();
    	//TX_Message();
    	GPS_NMEA_Data_Unpacker(&GPS);

    }

    return 0 ;
}

/*  Funcion de brujula  */
void Compass(void){

	float norm;

	/* Calculo de Roll y Pitch */
	Roll  = (float)atan2f(imu.Y, imu.Z) * (180.0f / _PI);
	Pitch = (float)atan2f(-1 * imu.X, sqrt(imu.Y*imu.Y + imu.Z*imu.Z)) * (180.0f / _PI);


	/* Compensacion y calculo de Yaw */
	norm = sqrt(imu.X*imu.X + imu.Y*imu.Y + imu.Z*imu.Z);

	float A_Pitch = -asin(imu.X / norm);
	float A_Roll  =  asin(imu.Y / cos(A_Pitch) / norm);

	norm = sqrt(mag.X*mag.X + mag.Y*mag.Y + mag.Z*mag.Z);

	float mx = mag.X/ norm;
	float my = -1 * mag.Y/ norm;
	float mz = mag.Z/ norm;

	float MX = mx * cos(A_Pitch) + mz * sin(A_Pitch);
	float MY = mx * sin(A_Roll) * sin(A_Pitch) + my * cos(A_Roll) - mz * sin(A_Roll) * cos(A_Pitch);

	Yaw = atan2f(-1 * MY, MX) * (180.0f / _PI);

	if(Yaw > 360) 		Yaw -= 360.0f;
	else if(Yaw < 0)	Yaw += 360.0f;

}

/*  Funcion que transmite datos via UART  */
void TX_Data(char data[], uint16_t size){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
		LPUART_WriteBlocking(LPUART0, (uint8_t*)data, size / sizeof(uint8_t));
	}
	txIndex = 0;
	rxIndex = 0;
}


/*  Funcion que transmite los datos de la NAVC por UART  */
void TX_Message(){

	char buffer[16];

	sprintf(buffer, "%3.2f %3.2f %3.2f", Pitch, Roll, Yaw);
	//PRINTF("%s \n", buffer);
	TX_Data(buffer, sizeof(buffer));

}


void Config_Port_Int(void){

	const port_pin_config_t port_int1_config = {
		.pullSelect = kPORT_PullDisable,
		.slewRate = kPORT_SlowSlewRate,
		.passiveFilterEnable = kPORT_PassiveFilterDisable,
		.driveStrength = kPORT_LowDriveStrength,
		.mux = kPORT_MuxAsGpio,
	};
	const gpio_pin_config_t gpio_int1_config = {
		.pinDirection = kGPIO_DigitalInput,
		.outputLogic = 0U
	};


	/*	Configuracion de interrupciones del sistema	  */


	PORT_SetPinConfig(MAG_INT1_PORT, MAG_INT1_PIN, &port_int1_config);

	GPIO_PinInit(MAG_INT1_GPIO, MAG_INT1_PIN, &gpio_int1_config);

	PORT_SetPinInterruptConfig(MAG_INT1_PORT, MAG_INT1_PIN, kPORT_InterruptRisingEdge);
	PORT_SetPinInterruptConfig(IMU_ACC_INT1_PORT, IMU_ACC_INT1_PIN, kPORT_InterruptLogicZero);
	PORT_SetPinInterruptConfig(IMU_GYR_INT2_PORT, IMU_GYR_INT2_PIN, kPORT_InterruptLogicZero);

	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable);

	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);

	EnableIRQ(LPUART0_IRQn);
	EnableIRQ(UART2_FLEXIO_IRQn);

	NVIC_SetPriority(UART2_FLEXIO_IRQn, 0);		// Interrupcion de UART2 (GPS).		Maxima prioridad, stream de datos
	NVIC_SetPriority(PORTC_PORTD_IRQn, 1);		// Interrupcion de Mag. 			Media prioridad, datos en registros
	NVIC_SetPriority(PORTA_IRQn, 1);			// Interrupcion de Acc y Gyr. 		Media prioridad, datos en registros
	NVIC_SetPriority(LPUART0_IRQn, 2);			// Interrupcion de LPUART0 (PC)		Baja prioridad, debug
}


/*  Handler de la interrupcion de perifericos en PORTC y PORTD, Mag  */
void PORTC_PORTD_IRQHandler(void)
{

	int16_t readM;

    // Leo flag de interrupcion
    uint32_t PORTD_int = PORT_GetPinsInterruptFlags(PORTD);
    uint32_t PORTC_int = PORT_GetPinsInterruptFlags(PORTC);

    if(PORTD_int && (1 << 3)){

    	readM   = (int16_t)QMC5883L_read_reg(0x01)<<8;
		readM  |= QMC5883L_read_reg(0x00);
		mag_readX = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x03)<<8;
		readM  |= QMC5883L_read_reg(0x02);
		mag_readY = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x05)<<8;
		readM  |= QMC5883L_read_reg(0x04);
		mag_readZ = readM;


    	PORT_ClearPinsInterruptFlags(MAG_INT1_PORT, 1<<MAG_INT1_PIN);
    }
}

/* Handler de la interrupcion de perifericos en PORTA, Acc y Gyr  */
void PORTA_IRQHandler(void){

	uint32_t PORTA_int = PORT_GetPinsInterruptFlags(PORTA);
	int16_t read;

    if(PORTA_int && (1 << IMU_ACC_INT1_PIN)){

    	// Leo aceleracion
    	read   = (int16_t)BMI160_read_reg(IMU_ACC_X_HI)<<8;
		read  |= BMI160_read_reg(IMU_ACC_X_LO);
		acc_readX = read >> 2;

		read   = (int16_t)BMI160_read_reg(IMU_ACC_Y_HI)<<8;
		read  |= BMI160_read_reg(IMU_ACC_Y_LO);
		acc_readY = read >> 2;

		read   = (int16_t)BMI160_read_reg(IMU_ACC_Z_HI)<<8;
		read  |= BMI160_read_reg(IMU_ACC_Z_LO);
		acc_readZ = read >> 2;

		// Leo giroscopio
		read   = (int16_t)BMI160_read_reg(IMU_GYR_X_HI)<<8;
		read  |= BMI160_read_reg(IMU_GYR_X_LO);
		gyr_readX = read >> 2;

		read   = (int16_t)BMI160_read_reg(IMU_GYR_Y_HI)<<8;
		read  |= BMI160_read_reg(IMU_GYR_Y_LO);
		gyr_readY = read >> 2;

		read   = (int16_t)BMI160_read_reg(IMU_GYR_Z_HI)<<8;
		read  |= BMI160_read_reg(IMU_GYR_Z_LO);
		gyr_readZ = read >> 2;

		// Leo magentometro
		read   = (int16_t)QMC5883L_read_reg(0x01)<<8;
		read  |= QMC5883L_read_reg(0x00);
		mag_readX = read;

		read   = (int16_t)QMC5883L_read_reg(0x03)<<8;
		read  |= QMC5883L_read_reg(0x02);
		mag_readY = read;

		read   = (int16_t)QMC5883L_read_reg(0x05)<<8;
		read  |= QMC5883L_read_reg(0x04);
		mag_readZ = read;

    	PORT_ClearPinsInterruptFlags(IMU_ACC_INT1_PORT, 1<<IMU_ACC_INT1_PIN);
    }

}


/* Handler de la interrupcion de LPUART0, conexión con PC  */
void LPUART0_IRQHandler(void)
{
    uint8_t data;
    uint16_t tmprxIndex = rxIndex;
    uint16_t tmptxIndex = txIndex;

    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART0))
    {
        data = LPUART_ReadByte(LPUART0);

        /* If ring buffer is not full, add data to ring buffer. */
        if (((tmprxIndex + 1) % RING_BUFFER_SIZE) != tmptxIndex)
        {
            RxBuffer[rxIndex] = data;
            rxIndex++;
            rxIndex %= RING_BUFFER_SIZE;
        }
    }

}







