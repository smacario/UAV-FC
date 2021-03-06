/*
 * 	GPS.c
 *
 *  Created on: 30 Oct 2020
 *      Author: Santiago Macario
 */

#include <stdio.h>
#include "board.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "fsl_lpuart.h"
#include "MKL43Z4.h"
#include "fsl_debug_console.h"
#include "GPS.h"

uint8_t UART2_RingBuffer[RING_BUFFER_SIZE];		// Buffer de datos recibidos
volatile uint16_t uart2_txIndex;				// Indice de transmision
volatile uint16_t uart2_rxIndex;				// Indice de recepcion

GPRCM_data GPRCM;								// Datos de la trama GPRCM
bool End_Of_Message = false;					// Flag de deteccion de finde mensaje


/*!
 * brief
 *
 */
void Tx_GPS(GPRCM_data GPS){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
		LPUART_WriteBlocking(LPUART0, GPS.Data, RING_BUFFER_SIZE / sizeof(uint8_t));
	}
}

/*!
 * brief 	Se procesa la trama del GPS
 *
 * 			Una vez que se reciben los datos del buffer, se verifica que la trama sea la correcta y luego
 * 			se extraen los datos pertienentes a cada una. En este caso GPRCM incluye Lat, Long, SOG, Fecha,
 * 			etc. Además se convierten las unidades de formato DDMM.MMMMMM y lo convierto a grados decimales
 * 			y la velocidad sobre el suelo de nudos a kmh.
 * 			Por ultimo se verifica la validez de la trama.
 *
 */
void GPS_NMEA_Data_Unpacker(GPS_Data *gps_data){

	if(End_Of_Message){

		// Verifico que los datos GPRCM sean validos, si lo son, calculo datos.
		if(GPRCM.Status == 'A' && GPRCM.SentenceName[2] == 'R' && GPRCM.SentenceName[3] == 'M' && GPRCM.SentenceName[4] == 'C'){

			if(!gps_data->DataValid) gps_data->DataValid = true;

			int DD;
			float MM;
			float lon_data, lat_data;

			// Leo datos de longitud y latitud en formato DDMM.MMMMMM y lo convierto a grados decimales.
			lat_data = strtod(GPRCM.Latitude, NULL);
			lon_data = strtod(GPRCM.Longitude, NULL);

			DD = (int)(lat_data/100);
			MM = lat_data - (DD * 100);
			if(GPRCM.Latitude_Orientation == 'S') {
				DD = DD*(-1);
				gps_data->Latitude = DD - MM/60;
			}
			else gps_data->Latitude = DD + MM/60;

			DD = (int)(lon_data/100);
			MM = lon_data - (DD * 100);
			if(GPRCM.Longitude_Orientation == 'W'){
				DD = DD*(-1);
				gps_data->Longitude = DD - MM/60;
			}
			else gps_data->Longitude = DD + MM/60;


			// Leo dato de velocidad y lo paso a GPS
			gps_data->Speed = strtod(GPRCM.SOG, NULL) * KNOTS_TO_KMS_CONVERTION;

		}
		else{
			if(gps_data->DataValid) gps_data->DataValid = false;
		}

		End_Of_Message = false;
	}
}

/*!
 * brief 	Handler de UART2 (GPS)
 *
 *			Se reciben los datos en un Ring Buffer, cuando se detecta el caracter de fin de trama (0x0A),
 * 			los datos del mismo se copian a la estructura de GPRCM, y se limpia el biffer anterior.
 *
 */
void UART2_FLEXIO_IRQHandler(void)
{
    uint8_t data;

    if ((kUART_RxDataRegFullFlag)&UART_GetStatusFlags(UART2)){

        UART_ReadBlocking(UART2, &data, 1);

        UART2_RingBuffer[uart2_rxIndex] = data;
		uart2_rxIndex++;

        /* Detecto final de trama <LF> */
        if(data == 0x0A && !End_Of_Message){

			End_Of_Message = true;
			uart2_rxIndex = 0;

			memcpy(GPRCM.Data, UART2_RingBuffer, sizeof(UART2_RingBuffer));
			memset(UART2_RingBuffer, 0x00, sizeof(UART2_RingBuffer));
        }
    }

}
