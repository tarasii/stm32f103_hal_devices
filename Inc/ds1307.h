#ifndef ds1307
#define ds1307
#include "i2c.h"
#include <stdbool.h>
#include <stdlib.h>
#define DS1307_I2C_ADDRESS 0x68 // the slave address (0xD0 read / 0xD1 write)
#define TIME_STRUCT_SIZE 0x08
#define DS1307_RAM_SIZE 55

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    //bool am;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    bool clock_halt;
    bool out;
    bool sqwe;
    bool rs1;
    bool rs0;
} DS1307_time;
// RS1 | RS0 | SQ output | SQWE | OUT
//  0  |  0  |    1 Hz   |   1  |  X
//  0  |  1  |  4.096 Hz |   1  |  X
//  1  |  0  |  8.192 Hz |   1  |  X
//  1  |  1  | 32.768 Hz |   1  |  X
//  X  |  X  |     0     |   0  |  0
//  X  |  X  |     1     |   0  |  1

HAL_StatusTypeDef DS1307_Init(void);
uint8_t* DS1307_ReadTime(void);
void DS1307_DecodeTime(const uint8_t *data, DS1307_time *s_time);
uint8_t* DS1307_EncodeData(const DS1307_time *s_time);
void DS1307_WriteTime(const DS1307_time *s_time);
void DS1307_PrintTime(const DS1307_time *s_time);
//void printByte(const uint8_t n);
//void printRawData(const uint8_t *data, const uint8_t size);
DS1307_time DS1307_GetTime(void);

// INFO: Addresses are relative!
// According to documentation:
// 0x00 - Timekeeper start
// 0x07 - Timekeeper end (8 bytes)
// 0x08 - RAM start
// 0x3F - RAM end (56 bytes)
// In this function - start address = 0x00 and end address = 0x38 (56 in dec)
uint8_t* DS1307_Read(const uint8_t start_address, const uint8_t bytes);
//void write(const uint8_t start_address, const uint8_t bytes, const bool ram, uint8_t* data);

//    u8 *ptr = read(TIME_STRUCT_SIZE, DS1307_RAM_SIZE);
//    printRawData(ptr, DS1307_RAM_SIZE);
//    free(ptr);

//    t1 = getTime();
//    printTime(&t1);

#endif // ds1307
