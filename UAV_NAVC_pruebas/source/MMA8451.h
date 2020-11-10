#ifndef MMA8451_H_
#define MMA8451_H_

#include "stdint.h"
#include "init_board.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MMA8451_I2C     		I2C0
#define MMA8451_I2C_ADDRESS     (0x1d)

#define ACC_INT1_PORT       	PORTC
#define ACC_INT1_GPIO       	GPIOC
#define ACC_INT1_PIN        	5

#define ACC_CTRL_REG1_ADDRESS   0X2A
#define ACC_CTRL_REG5_ADDRESS   0X2E
#define ACC_CTRL_REG4_ADDRESS   0X2D
#define ACC_INT_SOURCE_ADDRESS  0X0C
#define ACC_STATUS_ADDRESS      0X00
#define ACC_FF_MT_CFG_ADRESS	0x15
#define ACC_FF_MT_SRC_ADDRESS	0x16
#define ACC_FF_MT_THS_ADDRESS	0x17
#define ACC_FF_MT_COUNT_ADDRESS	0x18
#define X_OFFSET				0X2F
#define Y_OFFSET				0X30
#define Z_OFFSET				0X31

#define SENSITIVITY_4G 			4096



typedef union
{
    struct
    {
        unsigned SRC_DRDY:1;
        unsigned :1;
        unsigned SRC_FF_MT:1;
        unsigned SRC_PULSE:1;
        unsigned SRC_LNDPRT:1;
        unsigned SRC_TRANS:1;
        unsigned SRC_FIFO:1;
        unsigned SRC_ASLP:1;
    };
    uint8_t data;
}ACC_INT_SOURCE_t;


typedef union
{
    struct
    {
        unsigned XDR:1;
        unsigned YDR:1;
        unsigned ZDR:1;
        unsigned ZYXDR:1;
        unsigned XOW:1;
        unsigned YOW:1;
        unsigned ZOW:1;
        unsigned ZYXOW:1;
    };
    uint8_t data;
}ACC_STATUS_t	;


typedef union
{
    struct
    {
        unsigned :3;
        unsigned XEFE:1;
        unsigned YEFE:1;
        unsigned ZEFE:1;
        unsigned OAE:1;
        unsigned ELE:1;

    };
    uint8_t data;
}ACC_FF_MT_CFG_t	;


typedef union
{
    struct
    {
        unsigned ACTIVE:1;
        unsigned F_READ:1;
        unsigned LNOISE:1;
        unsigned DR:3;
        unsigned ASLP_RATE:2;
    };
    uint8_t data;
}ACC_CTRL_REG1_t;


typedef union
{
    struct
    {
        unsigned INT_EN_DRDY:1;
        unsigned :1;
        unsigned INT_EN_FF_MT:1;
        unsigned INT_EN_PULSE:1;
        unsigned INT_EN_LNDPRT:1;
        unsigned INT_EN_TRANS:1;
        unsigned INT_EN_FIFO:1;
        unsigned INT_EN_ASLP:1;
    };
    uint8_t data;
}ACC_CTRL_REG4_t;


typedef union
{
    struct
    {
        unsigned INT_CFG_DRDY:1;
        unsigned :1;
        unsigned INT_CFG_FF_MT:1;
        unsigned INT_CFG_PULSE:1;
        unsigned INT_CFG_LNDPRT:1;
        unsigned INT_CFG_TRANS:1;
        unsigned INT_CFG_FIFO:1;
        unsigned INT_CFG_ASLP:1;
    };
    uint8_t data;
}ACC_CTRL_REG5_t;


void mma8451_init(void);

uint8_t mma8451_read_reg(uint8_t addr);
void mma8451_write_reg(uint8_t addr, uint8_t data);
void config_port_int1(void);
void MMA8451_calibration(void);
void mma8451_setDataRate(DR_enum rate);
void acc_init(void);
void acc_stop();


#ifdef __cplusplus
}
#endif

#endif /* MMA8451_H_ */
