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

#include "fsl_lpuart.h"


uint8_t RingBuffer[RING_BUFFER_SIZE];
volatile uint16_t txIndex;									// Index of the data to send out.
volatile uint16_t rxIndex; 									// Index of the memory to save new arrived data.
PORT port = UART0;
uint8_t NAVC_DATA[8];

static int16_t imu_readX, imu_readY, imu_readZ;				// Lectura de acelerometro
static int32_t mag_readX, mag_readY, mag_readZ;				// Lectura de magnetometro

int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;			// Offset automatico magnetometro
int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;			// Offset automatico acelerometro

static float Pitch, Roll, Yaw;								// Angulos con respecto a lso ejes X, Y, Z


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

    /* Configuracion del puerto LPUART0 */
    lpuart_config_t config;

    uint16_t tmprxIndex = rxIndex;
    uint16_t tmptxIndex = txIndex;

	LPUART_GetDefaultConfig(&config);
	config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
	config.enableTx     = true;
	config.enableRx     = true;

	LPUART_Init(LPUART0, &config, LPUART_CLK_FREQ);


	/* Enable RX interrupt. */
	LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
	EnableIRQ(LPUART0_IRQn);

    while(1) {

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

    	compass();

    	// Transmito datos
    	TX_Message();


    	/* Actualizacion de RingBuffer (solamente usado para recibir datos) */
    	if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
			tmprxIndex = rxIndex;
			tmptxIndex = txIndex;
			if (tmprxIndex != tmptxIndex){
				txIndex++;
				txIndex %= RING_BUFFER_SIZE;
			}
		}

    	//PRINTF("NAVC_DATA %i %i %i %i %i %i \n", NAVC_DATA[0], NAVC_DATA[1], NAVC_DATA[2], NAVC_DATA[3], NAVC_DATA[4], NAVC_DATA[5]);
    }

    return 0 ;
}


void compass(void){

	float norm;

	/* Calculo de Roll y Pitch */
	Roll  = (float)atan2f(imu.Y, imu.Z) * (180.0f / _PI);
	Pitch = (float)atan2f(-1 * imu.X, sqrt(imu.Y*imu.Y + imu.Z*imu.Z)) * (180.0f / _PI);


	/* Compensacion y calculo de Yaw */
	norm = sqrt(imu.X*imu.X + imu.Y*imu.Y + imu.Z*imu.Z);

	float A_Pitch = -asin(imu.X / norm);
	float A_Roll  = asin(imu.Y / cos(A_Pitch) / norm);

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

void TX_Data(uint8_t data[], uint16_t size){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
		LPUART_WriteBlocking(LPUART0, data, size / sizeof(uint8_t));
	}
	txIndex = 0;
	rxIndex = 0;
}

void TX_Message(){

	// Armo el mensaje a transmitir, uso los bits 0 y 7 para inicio y fin de trama.
	// Los datos de 16 bits los separo en HIGH y LOW respectivamente.

	NAVC_DATA[0] = 60;	// Inicio de trama

	NAVC_DATA[1] = ((int16_t)Pitch) >> 8;
	NAVC_DATA[2] = ((int16_t)Pitch) & 0xFF;

	NAVC_DATA[3] = ((int16_t)Roll) >> 8;
	NAVC_DATA[4] = ((int16_t)Roll) & 0xFF;

	NAVC_DATA[5] = ((int16_t)Yaw) >> 8;
	NAVC_DATA[6] = ((int16_t)Yaw) & 0xFF;

	NAVC_DATA[7] = 61;	// Fin de trama

	TX_Data(NAVC_DATA, sizeof(NAVC_DATA));

}


void Config_Port_Int(void){

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
	// Interruption Handler para Magnetometro y Acelerometro

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
            RingBuffer[rxIndex] = data;
            rxIndex++;
            rxIndex %= RING_BUFFER_SIZE;
        }
    }

}







