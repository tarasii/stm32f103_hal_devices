#ifndef bmp180
#define bmp180

#include "i2c.h"
#include "math.h"

#define BMP180_I2C_ADDRESS 0xEE >> 1 // the slave address (0xEE read / 0xEF write)
#define BMP180_EEPROM_SIZE 22

/* Registers */
#define	BMP180_REGISTER_CONTROL 	0xF4
#define	BMP180_REGISTER_RESULT 		0xF6
#define BMP180_REGISTER_EEPROM		0xAA

/* Commands */
#define	BMP180_COMMAND_TEMPERATURE 	0x2E
#define	BMP180_COMMAND_PRESSURE_0 	0x34
#define	BMP180_COMMAND_PRESSURE_1 	0x74
#define	BMP180_COMMAND_PRESSURE_2 	0xB4
#define	BMP180_COMMAND_PRESSURE_3 	0xF4

/* Minimum waiting delay, in microseconds */
#define BMP180_TEMPERATURE_DELAY	4500
#define BMP180_PRESSURE_0_DELAY		4500
#define BMP180_PRESSURE_1_DELAY		7500
#define BMP180_PRESSURE_2_DELAY		13000
#define BMP180_PRESSURE_3_DELAY		25000


typedef enum {
	BMP180_Oversampling_UltraLowPower = 0x00,      /*!< 1 sample for result */
	BMP180_Oversampling_Standard = 0x01,           /*!< 2 samples for result */
	BMP180_Oversampling_HighResolution = 0x02,     /*!< 3 samples for result */
	BMP180_Oversampling_UltraHighResolution = 0x03 /*!< 4 samples for result */
} BMP180_Oversampling_t;


typedef struct {
	float Altitude;                        /*!< Calculated altitude at given read pressure */
	uint32_t Pressure;                     /*!< Pressure in pascals */
	float Temperature;                     /*!< Temperature in degrees */
	uint16_t Delay;                        /*!< Number of microseconds, that sensor needs to calculate data that you request to */
	BMP180_Oversampling_t Oversampling; /*!< Oversampling for pressure calculation */
} BMP180_t;


HAL_StatusTypeDef BMP180_Init(BMP180_t* BMP180_Data);
HAL_StatusTypeDef BMP180_StartTemperature(BMP180_t* BMP180_Data);
HAL_StatusTypeDef BMP180_ReadTemperature(BMP180_t* BMP180_Data);
HAL_StatusTypeDef BMP180_StartPressure(BMP180_t* BMP180_Data, BMP180_Oversampling_t Oversampling);
HAL_StatusTypeDef BMP180_ReadPressure(BMP180_t* BMP180_Data);
uint32_t BMP180_GetPressureAtSeaLevel(uint32_t pressure, float altitude);
float BMP180_GetHgPressure(uint32_t pressure);

#endif // bmp180
