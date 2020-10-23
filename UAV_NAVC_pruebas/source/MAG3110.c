#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "MAG3110.h"
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "init_board.h"
#include "fsl_debug_console.h"

uint8_t calibration_iter = 0;
uint16_t milisec = 0;
bool DataReady = false;				// Flag utilizada en calibracion de magnetometro

int16_t Xout_Mag_16_bit_avg, Yout_Mag_16_bit_avg, Zout_Mag_16_bit_avg;

uint8_t MAG3110_read_reg(uint8_t addr)
{
	i2c_master_transfer_t masterXfer;
    uint8_t ret;

	memset(&masterXfer, 0, sizeof(masterXfer));    // pone todo en cero (sizeof())
	masterXfer.slaveAddress = MAG3110_I2C_ADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &ret;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C0, &masterXfer);

	return ret;
}


void MAG3310_write_reg(uint8_t addr, uint8_t data)
{
	i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = MAG3110_I2C_ADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0, &masterXfer);
    //printf("Se escribio en 0x%02hhx el valor 0x%02hhx\n",addr,data);
}


void MAG3110_calibration(void){
	// Implementar calibracion de magnetometro

	PRINTF("Calibrando Magnetometro...\n Mueva la placa en todas direcciones \n");

	int16_t Xout_Mag_16_bit_max, Yout_Mag_16_bit_max, Zout_Mag_16_bit_max;
	int16_t Xout_Mag_16_bit_min, Yout_Mag_16_bit_min, Zout_Mag_16_bit_min;
	int16_t Xout_Mag_16_bit,     Yout_Mag_16_bit,     Zout_Mag_16_bit;

	int16_t readMag;

	while(calibration_iter < 100){

		if(DataReady){

			DataReady = false;

			readMag   = (int16_t)MAG3110_read_reg(0x01)<<8;
			readMag  |= MAG3110_read_reg(0x02);
			Xout_Mag_16_bit = readMag >> 2;

			readMag   = (int16_t)MAG3110_read_reg(0x03)<<8;
			readMag  |= MAG3110_read_reg(0x04);
			Yout_Mag_16_bit = readMag >> 2;

			readMag   = (int16_t)MAG3110_read_reg(0x05)<<8;
			readMag  |= MAG3110_read_reg(0x06);
			Zout_Mag_16_bit = readMag >> 2;

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


	Xout_Mag_16_bit_avg = (Xout_Mag_16_bit_max + Xout_Mag_16_bit_min) / 2;
	Yout_Mag_16_bit_avg = (Yout_Mag_16_bit_max + Yout_Mag_16_bit_min) / 2;
	Zout_Mag_16_bit_avg = (Zout_Mag_16_bit_max + Zout_Mag_16_bit_min) / 2;

/*
	// Todo esto implementa la correccion de offset del MAG3110, no anda.
	// Los registros de correccion son de 15 bits
	Xout_Mag_16_bit_avg <<= 1;
	Yout_Mag_16_bit_avg <<= 1;
	Zout_Mag_16_bit_avg <<= 1;

	// Stand-by para poder escribir la calibracion
	MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, 0x00);

	// Escribo los registro de iron-offset
	MAG3310_write_reg(0x09, (char)(Xout_Mag_16_bit_avg & 0xFF));
	MAG3310_write_reg(0x0A, (char)((Xout_Mag_16_bit_avg & (0xFF << 8)) >> 8));

	MAG3310_write_reg(0x0B, (char)(Yout_Mag_16_bit_avg & 0xFF));
	MAG3310_write_reg(0x0C, (char)((Yout_Mag_16_bit_avg & (0xFF << 8)) >> 8));

	MAG3310_write_reg(0x0D, (char)(Zout_Mag_16_bit_avg & 0xFF));
	MAG3310_write_reg(0x0E, (char)((Zout_Mag_16_bit_avg & (0xFF << 8)) >> 8));

	PRINTF("Listo!\n\n");
	PRINTF("Se escribio: \n");
	PRINTF("X-offset: %i \n", (int16_t)((MAG3110_read_reg(0x09)<<8)&MAG3110_read_reg(0x0A)));
	PRINTF("Y-offset: %i \n", (int16_t)((MAG3110_read_reg(0x0B)<<8)&MAG3110_read_reg(0x0C)));
	PRINTF("Z-offset: %i \n", (int16_t)((MAG3110_read_reg(0x0D)<<8)&MAG3110_read_reg(0x0E)));

	MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, MAG_CTRL_REG_1 | 0x01);
*/
}

int16_t X_offset(void) {
	return Xout_Mag_16_bit_avg;
}

int16_t Y_offset(void) {
	return Yout_Mag_16_bit_avg;
}

int16_t Z_offset(void) {
	return Zout_Mag_16_bit_avg;
}

void MAG3310_init(void){

	/* Inicializa el magnetometro en modo continuo a 10Hz*/

	MAG3310_write_reg(MAG_CTRL_REG2_ADDRESS, 0x80);
	MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, MAG_CTRL_REG_1 | 0x01);

	// Offsets de correccion
	MAG3110_calibration();


	uint16_t sysmod = MAG3110_read_reg(0x08);
	printf("sysmod: 0x%02hhx \n\n", sysmod);

}


void MAG3310_setDataRate(DR_enum rate)
{
	MAG_CTRL_REG1_t ctr_reg1;
    bool estAct;

    /* antes de modificar data rate es necesario poner ACTIVE = 0 */
    ctr_reg1.data = MAG3110_read_reg(MAG_CTRL_REG1_ADDRESS);

    /* guarda valor que tiene ACTIVE y luego pone a cero */
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;

    MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* actualiza DR y en la misma escritura va a restaurar ACTIVE */
	ctr_reg1.DR = rate;
	ctr_reg1.ACTIVE = estAct;

	MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* verificación */
	ctr_reg1.data = MAG3110_read_reg(0x2a);
}


void MAG3310_stop(){
	MAG3310_write_reg(MAG_CTRL_REG2_ADDRESS, 0x00);
	MAG3310_write_reg(MAG_CTRL_REG1_ADDRESS, 0x00);
}


void SysTick_Handler(void){

	milisec++;

	if(milisec == 250){
		milisec = 0;
		DataReady = true;
	}

}


