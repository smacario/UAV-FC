/*
 * UAV_NAVC_pruebas.h
 *
 *  Created on: 22 Oct 2020
 *      Author: Santiago
 */

#ifndef UAV_NAVC_PRUEBAS_H_
#define UAV_NAVC_PRUEBAS_H_

#define _PI      3.14159265358979f

#define X_MAG_OFFSET_STATIC		(int16_t)153
#define Y_MAG_OFFSET_STATIC		(int16_t)-180
#define Z_MAG_OFFSET_STATIC		(int16_t)-92

#define LPUART_CLK_FREQ CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC)
#define DEMO_LPUART_IRQHandler LPUART0_IRQHandler

#define LPUART_RING_BUFFER_SIZE 128
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

	int16_t X_gyr;
	int16_t Y_gyr;
	int16_t Z_gyr;

}Imu;


void Config_Port_Int(void);
void Compass(void);
void TX_Data(char data[], uint16_t size);
void TX_Message();


#endif /* UAV_NAVC_PRUEBAS_H_ */
