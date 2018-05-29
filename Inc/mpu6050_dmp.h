#ifndef _MPU6050_DMP_H_
#define _MPU6050_DMP_H_

#include "mpu6050.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))


uint8_t MPU6050_DMP_Init(void);
uint16_t MPU6050_dmpGetFIFOPacketSize(void); 

#endif /* _MPU6050_DMP_H_ */
