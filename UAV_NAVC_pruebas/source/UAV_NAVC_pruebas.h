/*
 * UAV_NAVC_pruebas.h
 *
 *  Created on: 22 Oct 2020
 *      Author: Santiago Macario
 */

#ifndef UAV_NAVC_PRUEBAS_H_
#define UAV_NAVC_PRUEBAS_H_

#define _PI      					3.14159265358979f
#define _SENS_TIME_INC				0.01f

#define X_MAG_OFFSET_STATIC			(int16_t)153
#define Y_MAG_OFFSET_STATIC			(int16_t)-180
#define Z_MAG_OFFSET_STATIC			(int16_t)-92

#define LPUART_CLK_FREQ 			CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC)
#define DEMO_LPUART_IRQHandler 		LPUART0_IRQHandler

#define LPUART_RING_BUFFER_SIZE 	128
#define LAST_INDEX (rxIndex - 1) 	% RING_BUFFER_SIZE

#define INT_PIN						2
#define INT_GPIO					GPIOB

#define FC_INT_PERIOD				(uint8_t)250


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


typedef union{

	struct{
		unsigned char 	start;				// Start byte
		float 			NAVC_Pitch;			// Valor de Pitch (4 bytes)
		float 			NAVC_Roll;			// Valor de Roll  (4 bytes)
		float 			NAVC_Yaw;			// Valor de Yaw	  (4 bytes)
		float 			NAVC_option			// (4 bytes)
		//unsigned char	stop;				// Stop byte

	};
	unsigned char 		NAVC_PRY[18];		// 13 bytes que contriene los valores de Pitch Roll y Yaw en
											// formato char para transmitir via UART, ademas de los bytes
											// que indican fin y principio de mensaje
}FC_data;


void Config_Port_Int(void);
void Compass(void);
void TX_Data(char data[], uint16_t size);
void TX_Message();
void TX_Data_FC(char data[], uint16_t size);
void TX_Message_FC();
void Gyr_Compass(void);
void Complementary_Filter();


#endif /* UAV_NAVC_PRUEBAS_H_ */
