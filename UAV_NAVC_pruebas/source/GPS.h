#ifndef GPS_H_
#define GPS_H_

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "fsl_lpuart.h"
#include "MKL43Z4.h"
#include "fsl_debug_console.h"

#define LPUART_CLK_FREQ CLOCK_GetFreq(BOARD_DEBUG_UART_CLKSRC)

#define RING_BUFFER_SIZE 128
#define LAST_INDEX (rxIndex - 1) % RING_BUFFER_SIZE


typedef union{
	struct {

		//($)(GPRMC)(,)(182129.00)(,)(A)(,)(3320.15690,S)(,)(06013.75117,W)(,)(0.195)(,,)(081120),,,A*71

		char S_Start;
		char SentenceName[5];
		unsigned :8;
		char Time[9];
		unsigned :1;
		char Status;
		unsigned :1;
		char Latitude[12];
		unsigned :1;
		char Longitude[13];
		unsigned :1;
		char SOG[5];
		unsigned :2;
		char Date[6];
	};

	uint8_t Data[RING_BUFFER_SIZE];

}GPRCM_data;

typedef struct{

	float Longitude;
	float Latitude;
	float Heading;
	float Speed;

}GPS_Data;



void Tx_GPS(GPRCM_data GPS);
void _GPS();



#endif /* GPS_H_ */
