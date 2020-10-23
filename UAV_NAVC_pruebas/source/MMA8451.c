#include <MMA8451.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "init_board.h"
#include "UAV_NAVC_pruebas.h"
#include "fsl_debug_console.h"

extern int8_t  X_Acc_Offset, Y_Acc_Offset, Z_Acc_Offset;

uint8_t mma8451_read_reg(uint8_t addr)
{
	i2c_master_transfer_t masterXfer;
    uint8_t ret;

	memset(&masterXfer, 0, sizeof(masterXfer));    // pone todo en cero (sizeof())
	masterXfer.slaveAddress = MMA8451_I2C_ADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &ret;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C0, &masterXfer);

	return ret;
}


void mma8451_write_reg(uint8_t addr, uint8_t data)
{
	i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = MMA8451_I2C_ADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0, &masterXfer);
    //printf("Se escribio en 0x%02hhx el valor 0x%02hhx\n",addr,data);
}


void mma8451_setDataRate(DR_enum rate)
{
	ACC_CTRL_REG1_t ctr_reg1;
    bool estAct;

    /* antes de modificar data rate es necesario poner ACTIVE = 0 */
    ctr_reg1.data = mma8451_read_reg(ACC_CTRL_REG1_ADDRESS);

    /* guarda valor que tiene ACTIVE y luego pone a cero */
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;

	mma8451_write_reg(ACC_CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* actualiza DR y en la misma escritura va a restaurar ACTIVE */
	ctr_reg1.DR = rate;
	ctr_reg1.ACTIVE = estAct;

	mma8451_write_reg(ACC_CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* verificación */
	ctr_reg1.data = mma8451_read_reg(0x2a);
}

void MMA8451_calibration(void){

	int16_t readAcc;
	int16_t Xout_Accel, Yout_Accel, Zout_Accel;

	PRINTF("Calibrando Acelerometro...\n");

	mma8451_write_reg(ACC_CTRL_REG1_ADDRESS, 0x00);					// Stand by

	// Leo datos
	readAcc   = (int16_t)mma8451_read_reg(0x01)<<8;
	readAcc  |= mma8451_read_reg(0x02);
	Xout_Accel = readAcc >> 2;

	readAcc   = (int16_t)mma8451_read_reg(0x03)<<8;
	readAcc  |= mma8451_read_reg(0x04);
	Yout_Accel = readAcc >> 2;

	readAcc   = (int16_t)mma8451_read_reg(0x05)<<8;
	readAcc  |= mma8451_read_reg(0x06);
	Zout_Accel = readAcc >> 2;

	// Calculo offset
	X_Acc_Offset = Xout_Accel / 8;
	Y_Acc_Offset = Yout_Accel / 8;
	Z_Acc_Offset = (Zout_Accel - SENSITIVITY_4G) / 8;
/*
	// Escribo registros
	mma8451_write_reg(X_OFFSET, X_Accel_offset);
	mma8451_write_reg(Y_OFFSET, Y_Accel_offset);
	mma8451_write_reg(Z_OFFSET, Z_Accel_offset);
*/
	PRINTF("Listo\n");
}


void acc_init(void){
	ACC_CTRL_REG1_t ctrl_reg1;
	ACC_CTRL_REG4_t ctrl_reg4;
	ACC_CTRL_REG5_t ctrl_reg5;

	ctrl_reg4.INT_EN_DRDY = 1;
	ctrl_reg4.INT_EN_FF_MT = 0;
	ctrl_reg4.INT_EN_PULSE = 0;
	ctrl_reg4.INT_EN_LNDPRT = 0;
	ctrl_reg4.INT_EN_TRANS = 0;
	ctrl_reg4.INT_EN_FIFO = 0;
	ctrl_reg4.INT_EN_ASLP = 0;
	mma8451_write_reg(ACC_CTRL_REG4_ADDRESS, ctrl_reg4.data);

	ctrl_reg5.INT_CFG_DRDY = 1;
	ctrl_reg5.INT_CFG_FF_MT = 0;
	ctrl_reg5.INT_CFG_PULSE = 0;
	ctrl_reg5.INT_CFG_LNDPRT = 0;
	ctrl_reg5.INT_CFG_TRANS = 0;
	ctrl_reg5.INT_CFG_FIFO = 0;
	ctrl_reg5.INT_CFG_ASLP = 0;
	mma8451_write_reg(ACC_CTRL_REG5_ADDRESS, ctrl_reg5.data);

	ctrl_reg1.ACTIVE = 1;
	ctrl_reg1.F_READ = 0;
	ctrl_reg1.LNOISE = 1;
	ctrl_reg1.DR = 0B101;
	ctrl_reg1.ASLP_RATE = 0B00;
    mma8451_write_reg(ACC_CTRL_REG1_ADDRESS, ctrl_reg1.data);



    MMA8451_calibration();



	mma8451_write_reg(ACC_CTRL_REG1_ADDRESS, ctrl_reg1.data);		// Activo

}


void acc_stop()
{
	mma8451_write_reg(0x2A, 0x0);
	config_port_int1();
}


/* EOF */
