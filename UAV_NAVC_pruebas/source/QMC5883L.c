#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "QMC5883L.h"
#include "peripherals.h"
#include "init_board.h"

#include "UAV_NAVC_pruebas.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

uint8_t calibration_iter = 0;								// Iteraciones para la autocalibracion
uint16_t milisec = 0;										// Contador para interrupcion de SysTick
bool DataReady = false;										// Flag utilizada en calibracion de magnetometro

extern int16_t X_Mag_Offset, Y_Mag_Offset, Z_Mag_Offset;	// Offset calculado en autocalibracion

i2c_master_handle_t g_m_handle_qmc5883;


uint8_t QMC5883L_read_reg(uint8_t addr)
{
	i2c_master_transfer_t masterXfer;
    uint8_t ret;

	memset(&masterXfer, 0, sizeof(masterXfer));    // pone todo en cero (sizeof())
	masterXfer.slaveAddress = QMC5883L_I2C_ADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &ret;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	//I2C_MasterTransferNonBlocking(I2C1_PERIPHERAL,&g_m_handle_qmc5883, &masterXfer);
	I2C_MasterTransferBlocking(I2C1_PERIPHERAL, &masterXfer);

	return ret;
}


void QMC5883L_write_reg(uint8_t addr, uint8_t data)
{
	i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = QMC5883L_I2C_ADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	//I2C_MasterTransferNonBlocking(I2C1_PERIPHERAL,&g_m_handle_qmc5883, &masterXfer);
    I2C_MasterTransferBlocking(I2C1_PERIPHERAL, &masterXfer);
    //PRINTF("Se escribio en 0x%02hhx el valor 0x%02hhx\n",addr,data);
}

/*
void QMC5883L_calibration(void){
	// Implementar calibracion de magnetometro

	PRINTF("Calibrando Magnetometro...\n Mueva la placa en todas direcciones \n");

	int16_t Xout_Mag_16_bit_max, Yout_Mag_16_bit_max, Zout_Mag_16_bit_max;
	int16_t Xout_Mag_16_bit_min, Yout_Mag_16_bit_min, Zout_Mag_16_bit_min;
	int16_t Xout_Mag_16_bit,     Yout_Mag_16_bit,     Zout_Mag_16_bit;

	int16_t readMag;

	while(calibration_iter < 100){

		if(DataReady){

			DataReady = false;

			readMag   = (int16_t)QMC5883L_read_reg(0x01)<<8;
			readMag  |= QMC5883L_read_reg(0x00);
			Xout_Mag_16_bit = readMag;

			readMag   = (int16_t)QMC5883L_read_reg(0x03)<<8;
			readMag  |= QMC5883L_read_reg(0x02);
			Yout_Mag_16_bit = readMag;

			readMag   = (int16_t)QMC5883L_read_reg(0x05)<<8;
			readMag  |= QMC5883L_read_reg(0x04);
			Zout_Mag_16_bit = readMag;

			if(calibration_iter == 0){

				Xout_Mag_16_bit_max = Xout_Mag_16_bit;
				Xout_Mag_16_bit_min = Xout_Mag_16_bit;

				Yout_Mag_16_bit_max = Yout_Mag_16_bit;
				Yout_Mag_16_bit_min = Yout_Mag_16_bit;

				Zout_Mag_16_bit_max = Zout_Mag_16_bit;
				Zout_Mag_16_bit_min = Zout_Mag_16_bit;
			}

			// Verifica si la muestra actual es la maxima

			if (Xout_Mag_16_bit > Xout_Mag_16_bit_max)    {Xout_Mag_16_bit_max = Xout_Mag_16_bit;}
			if (Xout_Mag_16_bit < Xout_Mag_16_bit_min)    {Xout_Mag_16_bit_min = Xout_Mag_16_bit;}


			if (Yout_Mag_16_bit > Yout_Mag_16_bit_max)    {Yout_Mag_16_bit_max = Yout_Mag_16_bit;}
			if (Yout_Mag_16_bit < Yout_Mag_16_bit_min)    {Yout_Mag_16_bit_min = Yout_Mag_16_bit;}


			if (Zout_Mag_16_bit > Zout_Mag_16_bit_max)    {Zout_Mag_16_bit_max = Zout_Mag_16_bit;}
			if (Zout_Mag_16_bit < Zout_Mag_16_bit_min)    {Zout_Mag_16_bit_min = Zout_Mag_16_bit;}

			calibration_iter++;
		}

	}


	X_Mag_Offset = (Xout_Mag_16_bit_max + Xout_Mag_16_bit_min) / 2;
	Y_Mag_Offset = (Yout_Mag_16_bit_max + Yout_Mag_16_bit_min) / 2;
	Z_Mag_Offset = (Zout_Mag_16_bit_max + Zout_Mag_16_bit_min) / 2;


	PRINTF("Listo! \n");

}
*/

void QMC5883L_init(void){

	// Soft reset
	QMC5883L_write_reg(MAG_CTRL2_REG_ADDRESS, 0x81);

	QMC5883L_write_reg(SET_RESET_REG_ADDRESS, 0x01);
	QMC5883L_write_reg(MAG_CTRL1_REG_ADDRESS, 0xD9);


	//QMC5883L_calibration();								// Rutina de calibracion

}



