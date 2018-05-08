
#ifndef I2C_UTILS_H_
#define I2C_UTILS_H_

#include "i2c.h"
#include "stdbool.h"


int8_t 	I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
int8_t 	I2Cdev_readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data);
int8_t 	I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
int8_t 	I2Cdev_readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
int8_t 	I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
int8_t 	I2Cdev_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data);
int8_t 	I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t 	I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

int8_t 	I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
int8_t 	I2Cdev_writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
int8_t 	I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
int8_t 	I2Cdev_writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
int8_t 	I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
int8_t 	I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
int8_t 	I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t 	I2Cdev_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);


#endif /* I2C_UTILS_H_ */ 
