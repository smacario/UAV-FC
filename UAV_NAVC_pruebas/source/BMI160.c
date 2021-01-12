/*
 * BMI160.c
 *
 *  Created on: 28 Nov 2020
 *      Author: Santiago Macario
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <BMI160.h>

#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "peripherals.h"

#include "init_board.h"
#include "UAV_NAVC_pruebas.h"
#include "fsl_debug_console.h"

i2c_master_handle_t g_m_handle;


/*!
 * brief 	Reads BMI160 register
 *
 */
uint8_t BMI160_read_reg(uint8_t addr)
{
	i2c_master_transfer_t masterXfer;
    uint8_t ret;

	memset(&masterXfer, 0, sizeof(masterXfer));    // pone todo en cero (sizeof())
	masterXfer.slaveAddress = BMI160_I2C_ADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &ret;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C1_PERIPHERAL, &masterXfer);
	//I2C_MasterTransferNonBlocking(I2C1_PERIPHERAL,&g_m_handle, &masterXfer);

	return ret;
}


/*!
 * brief 	Writes to BMI160 register
 *
 */
void BMI160_write_reg(uint8_t addr, uint8_t data)
{
	i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = BMI160_I2C_ADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C1_PERIPHERAL, &masterXfer);
	//I2C_MasterTransferNonBlocking(I2C1_PERIPHERAL,&g_m_handle, &masterXfer);
    //PRINTF("Se escribio en 0x%02x el valor 0x%02x\n",addr,data);
}


/*!
 * brief 	Checks BMI160 chip id
 *
 */
void BMI160_check_id(){
	uint8_t add;
	add = BMI160_read_reg(IMU_ID_ADDRESS);
	PRINTF("0x%02x \n", add);
}


/*!
 * brief 	Simple delay funcion to hold I2C bus
 *
 */
void delay(uint32_t cntr){
	while(cntr>0){
		cntr--;
		__NOP();
	}
}


/*!
 * brief 	Initializes BMI160 for continous reading on ACC and GYR, and enables
 * 			interrupt pin
 *
 */
void BMI160_init(void){

	uint8_t id = BMI160_read_reg(IMU_ID_ADDRESS);

	if(id != 0xD1){
		PRINTF("BMI160 no encontrado!\n");
	}
	else{

		// Soft Restart
		BMI160_write_reg(IMU_RA_CMD, BMI160_SOFT_RESET);
		delay(0xFFF);

		// Power up del acelerometro y gyroscopio
		BMI160_write_reg(IMU_RA_CMD, 0x11);
		delay(0xFFF);
		BMI160_write_reg(IMU_RA_CMD, 0x15);
		delay(0xFFF);

		// Configuraicon de Rango y ODR
		//BMI160_write_reg(IMU_ACC_CONF, 0x24);
		BMI160_write_reg(IMU_ACC_RANGE, 0x10);
		BMI160_write_reg(IMU_GYR_RANGE, 0x03);


		// Mapeo y habilitacion de interrupciones
		BMI160_write_reg(IMU_INT_MAP_2, 0x80);
		BMI160_write_reg(IMU_INT_OUT_CTRL, 0x08);
		BMI160_write_reg(IMU_INT_EN_1, 0x16);
	}
}











