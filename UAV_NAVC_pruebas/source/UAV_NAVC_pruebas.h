/*
 * UAV_NAVC_pruebas.h
 *
 *  Created on: 22 Oct 2020
 *      Author: Santiago
 */

#ifndef UAV_NAVC_PRUEBAS_H_
#define UAV_NAVC_PRUEBAS_H_

#define _PI      3.14159265358979f

typedef struct{
	int16_t X;
	int16_t Y;
	int16_t Z;
}Mag;

typedef struct{
	int16_t X;
	int16_t Y;
	int16_t Z;
}Imu;

void Config_Port_Int(void);

#endif /* UAV_NAVC_PRUEBAS_H_ */
