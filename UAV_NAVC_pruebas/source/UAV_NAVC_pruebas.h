/*
 * UAV_NAVC_pruebas.h
 *
 *  Created on: 22 Oct 2020
 *      Author: Santiago
 */

#ifndef UAV_NAVC_PRUEBAS_H_
#define UAV_NAVC_PRUEBAS_H_

#define _PI      3.14159265358979f

#define X_MAG_OFFSET_STATIC    	(int16_t)-1054
#define Y_MAG_OFFSET_STATIC		(int16_t) 1018
#define Z_MAG_OFFSET_STATIC		(int16_t) 552

#define LPUART_CLK_FREQ CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC)
#define DEMO_LPUART_IRQHandler LPUART0_IRQHandler

#define RING_BUFFER_SIZE 16
#define LAST_INDEX (rxIndex - 1) % RING_BUFFER_SIZE

typedef struct{
	int16_t X;
	int16_t Y;
	int16_t Z;
}Mag;

typedef struct{
	int16_t X;
	int16_t Y;
	int16_t Z;
}Imu;

typedef enum{						// Estados de Puertos disponibles
	UART0 = 0,
	UART1,
}PORT;

void Config_Port_Int(void);
void compass(void);
void TX_Data(uint8_t data[], uint16_t size);
void TX_Message();

#endif /* UAV_NAVC_PRUEBAS_H_ */
