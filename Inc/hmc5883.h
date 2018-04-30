#ifndef hmc5883
#define hmc5883

#include "i2c.h"
#include "math.h"


#define HMC5883L_I2C_READ_ADDRESS            0x3D 
#define HMC5883L_I2C_ADDRESS    (HMC5883L_I2C_READ_ADDRESS>>1)

#define HMC5883L_RA_CONFIG_A        0x00
#define HMC5883L_RA_CONFIG_B        0x01
#define HMC5883L_RA_MODE            0x02
#define HMC5883L_RA_DATAX_H         0x03
#define HMC5883L_RA_DATAX_L         0x04
#define HMC5883L_RA_DATAZ_H         0x05
#define HMC5883L_RA_DATAZ_L         0x06
#define HMC5883L_RA_DATAY_H         0x07
#define HMC5883L_RA_DATAY_L         0x08
#define HMC5883L_RA_STATUS          0x09
#define HMC5883L_RA_ID_A            0x0A
#define HMC5883L_RA_ID_B            0x0B
#define HMC5883L_RA_ID_C            0x0C

#define HMC5883L_CRA_AVERAGE_BIT    6
#define HMC5883L_CRA_AVERAGE_LENGTH 2
#define HMC5883L_CRA_AVERAGE_MASK   3
#define HMC5883L_CRA_AVERAGE_SHIFT  5
#define HMC5883L_CRA_RATE_BIT       4
#define HMC5883L_CRA_RATE_LENGTH    3
#define HMC5883L_CRA_RATE_MASK      7
#define HMC5883L_CRA_RATE_SHIFT     2
#define HMC5883L_CRA_BIAS_BIT       1
#define HMC5883L_CRA_BIAS_LENGTH    2
#define HMC5883L_CRA_BIAS_MASK      3

//#define HMC5883L_AVERAGING_1        0x00
//#define HMC5883L_AVERAGING_2        0x01
//#define HMC5883L_AVERAGING_4        0x02
//#define HMC5883L_AVERAGING_8        0x03

//#define HMC5883L_BIAS_NORMAL        0x00
//#define HMC5883L_BIAS_POSITIVE      0x01
//#define HMC5883L_BIAS_NEGATIVE      0x02

#define HMC5883L_CRB_GAIN_BIT       7
#define HMC5883L_CRB_GAIN_LENGTH    3
#define HMC5883L_CRB_GAIN_MASK    	7
#define HMC5883L_CRB_GAIN_SHIFT    	5

#define HMC5883L_MODEREG_BIT        1
#define HMC5883L_MODEREG_LENGTH     2
#define HMC5883L_MODEREG_MASK        3

#define HMC5883L_STATUS_LOCK_BIT    1
#define HMC5883L_STATUS_READY_BIT   0

typedef enum {
  HMC5883L_1SAMP = 0x00,//!< HMC6883L_1SAMP
  HMC5883L_2SAMP = 0x01,//!< HMC6883L_2SAMP
  HMC5883L_4SAMP = 0x02,//!< HMC6883L_4SAMP
  HMC5883L_8SAMP = 0x03,//!< HMC6883L_8SAMP
} HMC5883L_AvgSamples_TypeDef;
/**
 *
 */
typedef enum 
{                            /*!< Output Data Rate (Hz)  */
  HMC5883L_RATE_0P75 = 0x00, /*!<         0,1            */
  HMC5883L_RATE_1P5  = 0x01, /*!<         0,2            */
  HMC5883L_RATE_3    = 0x02, /*!<         0,39           */
  HMC5883L_RATE_7P5  = 0x03, /*!<         0,78           */
  HMC5883L_RATE_15   = 0x04, /*!<         15 (default)   */
  HMC5883L_RATE_30   = 0x05, /*!<         30             */
  HMC5883L_RATE_75   = 0x06  /*!<         75             */
} HMC5883L_DataRate_TypeDef;
/**
 *
 */
typedef enum
{                   
  HMC5883L_BIAS_NORMAL   = 0x00, /*!< Normal measurement configuration (default) */
  HMC5883L_BIAS_POSITIVE = 0x01, /*!< Positive bias configuration for X and Y axes, negative bias configuration for Z axis */
  HMC5883L_BIAS_NEGATIVE = 0x02  /*!< Negative bias configuration for X and Y axes, positive bias configuration for Z axis */
}HMC5883L_BIAS_TypeDef;
/**
 *
 */
typedef enum
{                    /*!< Sensor Input Field Range      Gain  */
  HMC5883L_GAIN_1280 = 0x00, /*!<        ± 0.9 Ga               1280  */
  HMC5883L_GAIN_1024 = 0x01, /*!<        ± 1.2 Ga (default)     1024  */
  HMC5883L_GAIN_768 = 0x02, /*!<        ± 1.9 Ga               768   */
  HMC5883L_GAIN_614 = 0x03, /*!<        ± 2.5 Ga               614   */
  HMC5883L_GAIN_415 = 0x04, /*!<        ± 4.0 Ga               415   */
  HMC5883L_GAIN_361 = 0x05, /*!<        ± 4.6 Ga               361   */
  HMC5883L_GAIN_307 = 0x06, /*!<        ± 5.5 Ga               307   */
  HMC5883L_GAIN_219 = 0x07  /*!<        ± 7.9 Ga               219   */
}HMC5883L_GainSettings_TypeDef ;    // Gain settings 
/**
 *
 */
typedef enum {
  HMC5883L_MODE_CONT = 0x00,  //!< HMC6883L_MODE_CONT
  HMC5883L_MODE_SINGLE = 0x01,//!< HMC6883L_MODE_SINGLE
  HMC5883L_MODE_IDLE = 0x02,  //!< HMC6883L_MODE_IDLE
  HMC5883L_MODE_SLEEP = 0x03,  //!< HMC6883L_MODE_SLEEP
} HMC5883L_Mode_TypeDef;
/**
 *
 */
typedef struct
{
	int16_t X;    //
	int16_t Y;    //
	int16_t Z;    //
}HMC5883L_XYZ_StructTypeDef; //   structure


HAL_StatusTypeDef HMC5883L_Initialize(void);
HAL_StatusTypeDef HMC5883L_TestConnection(void);


// CONFIG_A register
HMC5883L_AvgSamples_TypeDef HMC5883L_GetSampleAveraging(void);
void HMC5883L_SetSampleAveraging(HMC5883L_AvgSamples_TypeDef averaging);
HMC5883L_DataRate_TypeDef HMC5883L_GetDataRate(void);
void HMC5883L_SetDataRate(HMC5883L_DataRate_TypeDef rate);
uint8_t HMC5883L_GetMeasurementBias(void);
void HMC5883L_SetMeasurementBias(HMC5883L_BIAS_TypeDef bias);

// CONFIG_B register
HMC5883L_GainSettings_TypeDef HMC5883L_GetGain(void);
void HMC5883L_SetGain(HMC5883L_GainSettings_TypeDef gain);

void HMC5883L_Set_Configurarion(HMC5883L_AvgSamples_TypeDef sample, HMC5883L_DataRate_TypeDef rate, HMC5883L_BIAS_TypeDef bias, HMC5883L_GainSettings_TypeDef gain);

// MODE register
HMC5883L_Mode_TypeDef HMC5883L_GetMode(void);
void HMC5883L_SetMode(HMC5883L_Mode_TypeDef mode);

// STATUS register
uint8_t HMC5883L_GetLockStatus(void);
uint8_t HMC5883L_GetReadyStatus(void);

// DATA* registers
int16_t HMC5883L_GetX(void);
int16_t HMC5883L_GetY(void);
int16_t HMC5883L_GetZ(void);
void HMC5883L_GetXYZ(HMC5883L_XYZ_StructTypeDef *res);
double HMC5883L_CountAngle(HMC5883L_XYZ_StructTypeDef *res);

#endif // hmc5883
