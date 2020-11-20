
/*  Proyecto UAV-NAVC  */

#include <stdio.h>
#include "math.h"

#include "MKL43Z4.h"
#include "board.h"
#include "init_board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include <MMA8451.h>
#include <QMC5883L.h>
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

static int16_t imu_readX, imu_readY, imu_readZ;				// Lectura de acelerometro
static int32_t mag_readX, mag_readY, mag_readZ;				// Lectura de magnetometro

int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;			// Offset automatico magnetometro
int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;			// Offset automatico acelerometro

static float Pitch, Roll, Yaw;								// Angulos con respecto a los ejes X, Y, Z

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

    	// Compensacion de datos por calibracion automatica (ver implementacion por hardware)
/*
    	mag.X = mag_readX - X_Mag_Offset;
    	mag.Y = mag_readY - Y_Mag_Offset;
    	mag.Z = mag_readZ - Z_Mag_Offset;
*/
    	imu.X = imu_readX - X_Acc_Offset;
    	imu.Y = imu_readY - Y_Acc_Offset;
    	imu.Z = imu_readZ - Z_Acc_Offset;

    	// Compensacion de datos estaticos
    	mag.X = mag_readX - X_MAG_OFFSET_STATIC;
		mag.Y = mag_readY - Y_MAG_OFFSET_STATIC;
		mag.Z = mag_readZ - Z_MAG_OFFSET_STATIC;

    	Compass();
    	//TX_Message();
    	GPS_NMEA_Data_Unpacker(&GPS);
/*
    	if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
			tmprxIndex = rxIndex;
			tmptxIndex = txIndex;
			if (tmprxIndex != tmptxIndex){
				txIndex++;
				txIndex %= RING_BUFFER_SIZE;
			}
		}
*/
    	//PRINTF("NAVC_DATA_BUFFER %i %i %i %i %i %i \n", NAVC_DATA_BUFFER[0], NAVC_DATA_BUFFER[1], NAVC_DATA_BUFFER[2], NAVC_DATA_BUFFER[3], NAVC_DATA_BUFFER[4], NAVC_DATA_BUFFER[5]);
    }

    return 0 ;
}


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
	PORT_SetPinConfig(ACC_INT1_PORT, ACC_INT1_PIN, &port_int1_config);

	GPIO_PinInit(MAG_INT1_GPIO, MAG_INT1_PIN, &gpio_int1_config);
	GPIO_PinInit(ACC_INT1_GPIO, ACC_INT1_PIN, &gpio_int1_config);

	PORT_SetPinInterruptConfig(ACC_INT1_PORT, ACC_INT1_PIN, kPORT_InterruptLogicZero);
	PORT_SetPinInterruptConfig(MAG_INT1_PORT, MAG_INT1_PIN, kPORT_InterruptRisingEdge);

	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
	UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable);

	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	EnableIRQ(LPUART0_IRQn);
	EnableIRQ(UART2_FLEXIO_IRQn);

	NVIC_SetPriority(UART2_FLEXIO_IRQn, 0);		// Interrupcion de UART2 (GPS).		Maxima prioridad, stream de datos
	NVIC_SetPriority(PORTC_PORTD_IRQn, 1);		// Interrupcion de Mag y Acc. 		Media prioridad, datos en registros
	NVIC_SetPriority(LPUART0_IRQn, 2);			// Interrupcion de LPUART0 (PC)		Baja prioridad, debug
}


/* Handler de la interrupcion de perifericos en PORTC y PORTD, Mag y Acc  */
void PORTC_PORTD_IRQHandler(void)
{
	// Interruption Handler para Magnetometro y Acelerometro

    int16_t readG, readM;

    ACC_INT_SOURCE_t 	intSource;
    ACC_STATUS_t 		acc_status;
    MAG_STATUS_t 		mag_status;

    // Leo flag de interrupcion
    uint32_t PORTC_int = PORT_GetPinsInterruptFlags(PORTC);
    uint32_t PORTD_int = PORT_GetPinsInterruptFlags(PORTD);

    mag_status.data = QMC5883L_read_reg(MAG_STATUS_FLAG_ADDRESS);


    if(PORTC_int && (1 << 5)){

        intSource.data  = mma8451_read_reg(ACC_INT_SOURCE_ADDRESS);

    	if (intSource.SRC_DRDY){
			acc_status.data = mma8451_read_reg(ACC_STATUS_ADDRESS);

			if (acc_status.XDR){
				readG   = (int16_t)mma8451_read_reg(0x01)<<8;
				readG  |= mma8451_read_reg(0x02);
				imu_readX = readG >> 2;

				readM   = (int16_t)QMC5883L_read_reg(0x01)<<8;
				readM  |= QMC5883L_read_reg(0x00);
				mag_readX = readM;
			}

			if (acc_status.YDR){
				readG   = (int16_t)mma8451_read_reg(0x03)<<8;
				readG  |= mma8451_read_reg(0x04);
				imu_readY = readG >> 2;

				readM   = (int16_t)QMC5883L_read_reg(0x03)<<8;
				readM  |= QMC5883L_read_reg(0x02);
				mag_readY = readM;
			}

			if (acc_status.ZDR){
				readG   = (int16_t)mma8451_read_reg(0x05)<<8;
				readG  |= mma8451_read_reg(0x06);
				imu_readZ = readG >> 2;

				readM   = (int16_t)QMC5883L_read_reg(0x05)<<8;
				readM  |= QMC5883L_read_reg(0x04);
				mag_readZ = readM;
			}
		}

    	PORT_ClearPinsInterruptFlags(ACC_INT1_PORT, 1<<ACC_INT1_PIN);
    }
/*
    if (mag_status.DRDY || mag_status.DOR){

		readM   = (int16_t)QMC5883L_read_reg(0x01)<<8;
		readM  |= QMC5883L_read_reg(0x00);
		mag_readX = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x03)<<8;
		readM  |= QMC5883L_read_reg(0x02);
		mag_readY = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x05)<<8;
		readM  |= QMC5883L_read_reg(0x04);
		mag_readZ = readM;
	}
*/

    if(PORTD_int && (1 << 3)){
/*
    	readM   = (int16_t)QMC5883L_read_reg(0x01)<<8;
		readM  |= QMC5883L_read_reg(0x00);
		mag_readX = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x03)<<8;
		readM  |= QMC5883L_read_reg(0x02);
		mag_readY = readM;

		readM   = (int16_t)QMC5883L_read_reg(0x05)<<8;
		readM  |= QMC5883L_read_reg(0x04);
		mag_readZ = readM;

    	PRINTF("\n\n INTERRUPCION MAGNETOMETRO \n\n");}
*/
    	PORT_ClearPinsInterruptFlags(MAG_INT1_PORT, 1<<MAG_INT1_PIN);
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







