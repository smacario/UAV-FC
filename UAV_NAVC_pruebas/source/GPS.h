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

#define KNOTS_TO_KMS_CONVERTION	1.851999;

#define RING_BUFFER_SIZE 128
#define LAST_INDEX (rxIndex - 1) % RING_BUFFER_SIZE


typedef union{
	struct {

		//($)(GPRMC)(,)(182129.00)(,)(A)(,)(3320.15690)(,)(S)(,)(06013.75117)(,)(W)(,)(0.195)(,)(,)(081120),,,A*71

		char S_Start;
		char SentenceName[5];
		char :1;
		char Time[9];
		char :1;
		char Status;
		char :1;
		char Latitude[10];
		char :1;
		char Latitude_Orientation;
		char :1;
		char Longitude[11];
		char :1;
		char Longitude_Orientation;
		char :1;
		char SOG[5];
		char :2;
		char Date[6];
	};

	uint8_t Data[RING_BUFFER_SIZE];

}GPRCM_data;

typedef struct{

	float Longitude;
	float Latitude;
	float Heading;
	float Speed;
	float height;

	bool DataValid;


}GPS_Data;



void Tx_GPS(GPRCM_data GPS);
void GPS_NMEA_Data_Unpacker(GPS_Data *gps_data);



#endif /* GPS_H_ */
