/*
 * This section describes the process of reading from and writing to this device. The devices uses an address pointer to indicate which
 * register location is to be read from or written to. These pointer locations are sent from the master to this slave device and succeed
 * the 7-bit address (0x1E) plus 1 bit read/write identifier, i.e. 0x3D for read and 0x3C for write.
 *
 * To minimize the communication between the master and this device, the address pointer updated automatically without master intervention.
 * The register pointer will be incremented by 1 automatically after the current register has been read successfully.
 *
 * To move the address pointer to a random register location, first issue a “write” to that register location with no data byte following
 * the commend. For example, to move the address pointer to register 10, send 0x3C 0x0A.
 *
 */

#ifndef QMC5883L_H_
#define QMC5883L_H_


#define QMC5883L_I2C   				I2C1_PERIPHERAL
#define QMC5883L_I2C_ADDRESS    	(0x0D)

#define MAG_INT1_PORT       		PORTD
#define MAG_INT1_GPIO       		GPIOD
#define MAG_INT1_PIN        		3

#define MAG_STATUS_FLAG_ADDRESS		0x06
#define MAG_CTRL1_REG_ADDRESS		0x09
#define MAG_CTRL2_REG_ADDRESS		0x0A
#define SET_RESET_REG_ADDRESS		0x0B


typedef union
{
    struct
    {
        unsigned DRDY:1;
        unsigned OVL:1;
        unsigned DOR:1;
        unsigned :5;
    };
    uint8_t data;
}MAG_STATUS_t	;


uint8_t QMC5883L_read_reg(uint8_t addr);
void QMC5883L_write_reg(uint8_t addr, uint8_t data);
void QMC5883L_init(void);


#endif /* QMC5883L_H_ */
