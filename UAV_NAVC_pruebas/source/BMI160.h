/*
 * BMI160.h
 *
 *  Created on: 28 Nov 2020
 *      Author: Santiago
 */

#ifndef BMI160_H_
#define BMI160_H_


#define BMI160_I2C				I2C1_PERIPHERAL
#define BMI160_I2C_ADDRESS		(0x69)

#define IMU_ACC_INT1_PORT       PORTA
#define IMU_ACC_INT1_GPIO       GPIOA
#define IMU_ACC_INT1_PIN        12

#define IMU_GYR_INT2_PORT       PORTA
#define IMU_GYR_INT2_GPIO       GPIOA
#define IMU_GYR_INT2_PIN        5


#define IMU_ID_ADDRESS			0x00
#define IMU_ERR_REG				0x02
#define IMU_PMU_STATUS			0x03


#define IMU_GYR_X_LO			0x0C
#define IMU_GYR_X_HI			0x0D
#define IMU_GYR_Y_LO			0x0E
#define IMU_GYR_Y_HI			0x0F
#define IMU_GYR_Z_LO			0x10
#define IMU_GYR_Z_HI			0x11
#define IMU_ACC_X_LO			0x12
#define IMU_ACC_X_HI			0x13
#define IMU_ACC_Y_LO			0x14
#define IMU_ACC_Y_HI			0x15
#define IMU_ACC_Z_LO			0x16
#define IMU_ACC_Z_HI			0x17


#define IMU_STATUS				0x1B

#define IMU_ACC_CONF			0x40
#define IMU_ACC_RANGE			0x41
#define IMU_GYR_CONF			0x42
#define IMU_GYR_RANGE			0x43

#define IMU_INT_EN_0			0x50
#define IMU_INT_EN_1			0x51
#define IMU_INT_EN_2			0x52
#define IMU_INT_OUT_CTRL		0x53
#define IMU_INT_LATCH			0x54
#define IMU_INT_MAP_1			0x55
#define IMU_INT_MAP_2			0x56
#define IMU_INT_MAP_3			0x57

#define IMU_IF_CONF				0x6B
#define IMU_ACC_OFFSET_X		0x71
#define IMU_ACC_OFFSET_Y		0x72
#define IMU_ACC_OFFSET_Z		0x73
#define IMU_GYR_OFFSET_X		0x74
#define IMU_GYR_OFFSET_Y		0x75
#define IMU_GYR_OFFSET_Z		0x76
#define IMU_OFFSET_EN			0x77
#define IMU_RA_CMD				0x7E


// MACROS BMI160
#define BMI160_SOFT_RESET		0xB6


void BMI160_init(void);
void delay(uint32_t cntr);
uint8_t BMI160_read_reg(uint8_t addr);
void BMI160_write_reg(uint8_t addr, uint8_t data);
void BMI160_check_id();


#endif /* BMI160_H_ */
