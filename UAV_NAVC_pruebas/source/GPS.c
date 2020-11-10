/*
 * GPS.c
 *
 *  Created on: 9 Nov 2020
 *      Author: Santiago
 */

#include <stdio.h>
#include "board.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "fsl_lpuart.h"
#include "MKL43Z4.h"
#include "fsl_debug_console.h"
#include "GPS.h"

uint8_t UART2_RingBuffer[RING_BUFFER_SIZE];
volatile uint16_t uart2_txIndex;
volatile uint16_t uart2_rxIndex;

GPRCM_data GPRCM;							// Datos de la trama GPRCM.


bool End_Of_Message = false;				// Flag de deteccion de finde mensaje


void Tx_GPS(GPRCM_data GPS){
	if(kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)){
		LPUART_WriteBlocking(LPUART0, GPS.Data, RING_BUFFER_SIZE / sizeof(uint8_t));
	}
}


void _GPS(){

	if(End_Of_Message){
		End_Of_Message = false;
/*
		if(gprcm_data->Status == 'A' && gprcm_data->SentenceName[2] == 'R'){
			Tx_GPS(GPRCM);
		}
		else{
			__NOP();	// Implementar handler para invalidez de datos GPS
		}
*/
		Tx_GPS(GPRCM);
	}
}


void UART2_FLEXIO_IRQHandler(void)
{
    uint8_t data;

    if ((kUART_RxDataRegFullFlag)&UART_GetStatusFlags(UART2)){

        UART_ReadBlocking(UART2, &data, 1);

        UART2_RingBuffer[uart2_rxIndex] = data;
		uart2_rxIndex++;

        /* Detecto final de trama <LF>*/
        if(data == 0x0A){

			End_Of_Message = true;
			uart2_rxIndex = 0;

			memcpy(GPRCM.Data, UART2_RingBuffer, sizeof(UART2_RingBuffer));
			memset(UART2_RingBuffer, 0x00, sizeof(UART2_RingBuffer));
        }
    }

}
