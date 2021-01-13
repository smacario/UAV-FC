
/*
*	Programa de Computadora de NAVegacion (NAV-C) para poryecto UAV
*	Autor: Santiago Macario.
*	Sistemas Digitales III - FCEIA - UNR
*
*	https://github.com/smacario/UAV-FC.git
*
*	Problemas:
*		10/11/2020 - Interrupcion y flag de DRDY del Q5883L no funciona.
*		11/01/2021 - Heap overflow en I2C_MasterTransferBlocking. Cuelga el programa. (Implementar NonBlocking)
*
*
*/

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


//#define STATIC_OFFSET


uint8_t RxBuffer[LPUART_RING_BUFFER_SIZE];					// Buffer de Rx para LPUART0
volatile uint16_t txIndex;									// Index de Tx para LPUART0
volatile uint16_t rxIndex; 									// Index de Rx para LPUART0
uint8_t NAVC_DATA_BUFFER[12];								// Buffer de datos de la NAVC para LPUART0
bool FCIntReady = false;									// Flag para activar la interrupcion de datos a la FC para LPUART0
static uint16_t FCint_milisec;								// Contador de interrupcion en milisegundos

static int16_t acc_readX, acc_readY, acc_readZ;				// Lectura diracta del acelerometro
static int32_t mag_readX, mag_readY, mag_readZ;				// Lectura diracta del magnetometro
static int16_t gyr_readX, gyr_readY, gyr_readZ;				// Lectura diracta del giroscopio

int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;			// Offset automatico magnetometro
int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;			// Offset automatico acelerometro

static float Pitch, Roll, Yaw;								// Angulos con respecto a los ejes X, Y, Z
static float Gyr_Pitch,  Gyr_Roll,  Gyr_Yaw;				// Angulos con respecto a los ejes X, Y, Z de giroscopio
static float CF_Pitch, CF_Roll, CF_Yaw;						// Angulos calculados por el filtro complementario

Mag mag;													// Datos del magnetometro
Imu imu;													// Datos del acelerometro
GPS_Data GPS;												// Datos del GPS



int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    SysTick_Config(SystemCoreClock / 1000U);

    Board_Init();
    Config_Port_Int();

    while(1){

#ifndef STATIC_OFFSET
    	mag.X = mag_readX - X_Mag_Offset;
    	mag.Y = mag_readY - Y_Mag_Offset;
    	mag.Z = mag_readZ - Z_Mag_Offset;
#else
		mag.X = mag_readX - X_MAG_OFFSET_STATIC;
		mag.Y = mag_readY - Y_MAG_OFFSET_STATIC;
		mag.Z = mag_readZ - Z_MAG_OFFSET_STATIC;
#endif

    	imu.X = acc_readX - X_Acc_Offset;
    	imu.Y = acc_readY - Y_Acc_Offset;
    	imu.Z = acc_readZ - Z_Acc_Offset;


    	Compass();
    	//Gyr_Compass();
    	//Complementary_Filter();
    	TX_Message_FC();
    	GPS_NMEA_Data_Unpacker(&GPS);

    }
    return 0 ;
}


/*!
 * brief 	Calcula los angulos de giro con respecto a los ejes X, Y, Z.
 *
 * 			A partir de los datos del acelerometro se calculan los valores de Pitch y Roll,
 * 			para calcular el Yaw se usan los datos del magnetometro pero compensando la
 * 			inclinacion del sensor en todos sus angulos posibles.
 *
 */
void Compass(void){

	float norm;

	// Calculo de Roll y Pitch
	Roll  = (float)atan2f(imu.Y, imu.Z) * (180.0f / _PI);
	Pitch = (float)atan2f(-1 * imu.X, sqrt(imu.Y*imu.Y + imu.Z*imu.Z)) * (180.0f / _PI);


	// Compensacion y calculo de Yaw
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


/*!
 * brief 	Calcula los angulos de giro con respecto a los ejes X, Y, Z.
 *
 * 			Con los datos proporcionados por el giroscopio calcula los angulos de giro
 * 			con respecto a los ejes X, Y, Z.
 *
 *
 * 			Revisar implementacion, produce erroes en bus i2C
 *
 */
void Gyr_Compass(void){
	static uint32_t count = 0;
	static uint32_t prev_count = 0;

	count = (uint32_t)BMI160_read_reg(IMU_SENS_TIME_3) << 16;
	count |= (uint32_t)BMI160_read_reg(IMU_SENS_TIME_2) << 8;
	count |= (uint32_t)BMI160_read_reg(IMU_SENS_TIME_1);

	count = count - prev_count;

	Gyr_Roll  = imu.X_gyr * (float)count * _SENS_TIME_INC;
	Gyr_Pitch = imu.Y_gyr * (float)count * _SENS_TIME_INC;
	Gyr_Yaw   = imu.Z_gyr * (float)count * _SENS_TIME_INC;

	prev_count = count;
}


/* Calculo del filtro complementario */
void Complementary_Filter(){
	CF_Yaw = 0.98f * (CF_Yaw + Gyr_Yaw) + 0.02f * Yaw;
}


/*!
 * brief 	Transmite datos via puerto LPUART0 a PC
 *
 * 			param data: Bloque de datos a transmitr.
 * 			param size: Tamaño del bloque, (generalmente toma el valor sizeof(data))
 *
 */
void TX_Data(char data[], uint16_t size){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
		LPUART_WriteBlocking(LPUART0, (uint8_t*)data, size / sizeof(uint8_t));
	}
	txIndex = 0;
	rxIndex = 0;
}


/*!
 * brief 	Se construye el mensaje de LPUART0 y se envia con TX_Data()
 *
 */
void TX_Message(){
	char buffer[16];

	sprintf(buffer, "%3.2f %3.2f %3.2f", Pitch, Roll, Yaw);
	//PRINTF("%s \n", buffer);
	TX_Data(buffer, sizeof(buffer));

}


/*!
 * brief 	Transmite datos via puerto LPUART1 a FC
 *
 * 			param data: Bloque de datos a transmitr.
 * 			param size: Tamaño del bloque, (generalmente toma el valor sizeof(data))
 *
 */
void TX_Data_FC(char data[], uint16_t size){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1)){
		LPUART_WriteBlocking(LPUART1, (uint8_t*)data, size / sizeof(uint8_t));
	}
	txIndex = 0;
	rxIndex = 0;
}


/*!
 * brief 	Se construye el mensaje de LPUART1 para FC, y se envia con TX_Data_FC()
 *
 * 			Priemro se deshabilitan las interrupciones de LPUART y GPIO para no interrumpir
 *			la transmision, luego se da una señal de interrupcion para la FC por 25 ciclos
 * 			de clock, se transmite el mensaje y se vuelven a habilitar las inerrupciones.
 *
 */
void TX_Message_FC(){

	FC_data data;
	data.NAVC_Pitch 	= Pitch;
	data.NAVC_Roll  	= Roll;
	data.NAVC_Yaw		= Yaw;
	data.NAVC_option	= 123.56f;

	char buffer[sizeof(data.NAVC_PRY)];
	memset(buffer, 0x00, sizeof(buffer));

	// Deshabilito interrupciones para transmision sin interferencias
	LPUART_DisableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
	UART_DisableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable);
	NVIC_DisableIRQ(PORTC_PORTD_IRQn);
	NVIC_DisableIRQ(PORTA_IRQn);

	if(FCIntReady){
		// Pulso de 25 ciclos de reloj
		GPIO_PortClear(INT_GPIO, 1<<INT_PIN);
		for(uint8_t i=0 ; i<10 ; i++) __asm("NOP");
		GPIO_PortSet(INT_GPIO, 1<<INT_PIN);

		//sprintf(buffer, "$%s", data.NAVC_PRY);
		memcpy(buffer, data.NAVC_PRY, sizeof(buffer));
		buffer[0] 	= '$';

		TX_Data_FC(buffer, sizeof(buffer));

		// Reset de contador de periodo
		FCIntReady = false;
		FCint_milisec = 0;
	}

	// Habilito interrupciones externas
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);
	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable);
}


/*!
 * brief 	Se configuran las interupciones del sistema
 *
 */
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

	NVIC_SetPriority(SysTick_IRQn, 0);			// Interrupcion del Systic			Maxima prioridad. Stream de datos
	NVIC_SetPriority(UART2_FLEXIO_IRQn, 1);		// Interrupcion de UART2 (GPS).		Alta prioridad, stream de datos
	NVIC_SetPriority(PORTC_PORTD_IRQn, 2);		// Interrupcion de Mag. 			Media prioridad, datos en registros
	NVIC_SetPriority(PORTA_IRQn, 2);			// Interrupcion de Acc y Gyr. 		Media prioridad, datos en registros
	NVIC_SetPriority(LPUART0_IRQn, 3);			// Interrupcion de LPUART0 (PC)		Baja prioridad, debug
}


/*!
 * brief 	Handler de la interrupcion de perifericos en PORTC y PORTD, Mag
 *
 *			Se leen los datos del magnetometro con la interrupcion de PTD3
 *
 * 			Problemas:
 * 				No funciona la interrupcion en el sensor
 *
 */
void PORTC_PORTD_IRQHandler(void)
{

	int16_t readM;

    // Leo flag de interrupcion
    uint32_t PORTD_int = PORT_GetPinsInterruptFlags(PORTD);

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


/*!
 * brief 	Handler de la interrupcion de perifericos en PORTA, Acc y Gyr
 *
 * 			Ante la llegada de la interrupcion del BMI160, se procede a leer los datos de todos
 * 			los sensores, Gyr, Acc, y Mag. Este ultimo se asegura una actualizacion de datos mas
 * 			rapida para asegurar datos buenos. (No puede leerse la flag de DRDY en magnetometro)
 *
 */
void PORTA_IRQHandler(void){

	uint32_t PORTA_int = PORT_GetPinsInterruptFlags(PORTA);
	int16_t read;

    if(PORTA_int && (1 << IMU_ACC_INT1_PIN)){

    	// Leo acelerometro
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


/*!
 * brief 	Handler de la interrupcion de LPUART0, conexión con PC
 *
 */
void LPUART0_IRQHandler(void)
{
    uint8_t data;
    uint16_t tmprxIndex = rxIndex;
    uint16_t tmptxIndex = txIndex;

    // Nuevos datos en buffer
    if ((kLPUART_RxDataRegFullFlag) & LPUART_GetStatusFlags(LPUART0))
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


/*!
 * brief 	Handler de la interrupcion del Systick
 *
 * 			Se determiana la frecuencia de envio de datos a la FC con FC_INT_PERIOD.
 * 			Además se hace toggle en el led rojo de la placa para verificar
 *
 */
void SysTick_Handler(void){

	FCint_milisec++;

	if(FCint_milisec == FC_INT_PERIOD){
		FCIntReady = true;
		GPIO_PortToggle(BOARD_LED_ROJO_GPIO, 1<<BOARD_LED_ROJO_PIN);
	}

}





