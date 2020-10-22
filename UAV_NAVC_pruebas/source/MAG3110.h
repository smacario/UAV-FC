#include "stdint.h"
#include "init_board.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef MAG3110_H_
#define MAG3110_H_

#define MAG3110_I2C     		I2C0
#define MAG3110_I2C_ADDRESS     (0x0E)

#define MAG_INT1_PORT       	PORTD
#define MAG_INT1_GPIO       	GPIOD
#define MAG_INT1_PIN        	1

#define MAG_CTRL_REG1_ADDRESS   0X10
#define MAG_CTRL_REG2_ADDRESS   0X11
#define MAG_STATUS_ADDRESS		0x00

#define MAG_CTRL_REG_1			0x30


typedef union
{
    struct
    {
        unsigned ACTIVE:1;
        unsigned TRIGGER_M:1;
        unsigned FAST_READ:1;
        unsigned OS:2;
        unsigned DR:3;
    };
    uint8_t data;
}MAG_CTRL_REG1_t;

typedef union
{
    struct
    {
        unsigned :4;
        unsigned RESET:1;
        unsigned RAW:1;
        unsigned :1;
        unsigned AUTO_RESET:1;
    };
    uint8_t data;
}MAG_CTRL_REG2_t;

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
}MAG_STATUS_t	;


uint8_t MAG3110_read_reg(uint8_t addr);
void MAG3310_write_reg(uint8_t addr, uint8_t data);
void MAG3110_calibration(void);
void MAG3310_init(void);
void MAG3310_setDataRate(DR_enum rate);
int16_t X_offset(void);
int16_t Y_offset(void);
int16_t Z_offset(void);
void MAG3310_stop();


#endif /* MAG3110_H_ */






