#include "hmc5883.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t HMC5883L_lib_initialized = 0;
HMC5883L_Mode_TypeDef HMC5883Lmode;

uint8_t HMC5883L_ByteRead(uint8_t adress) {
	if (HMC5883L_lib_initialized == 0){
		return 0;
	}
  uint8_t received_data;
  I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, adress, &received_data, 1);
  return received_data;
}
	
void HMC5883L_ByteWrite(uint8_t adress, uint8_t data) {
	if (HMC5883L_lib_initialized == 0){
		return;
	}
  I2C_WriteBuf(&hi2c1, HMC5883L_I2C_ADDRESS, adress, &data, 1);
}

HAL_StatusTypeDef HMC5883L_Initialize(void)
{
	HMC5883L_lib_initialized = 1;
	if (HMC5883L_TestConnection() != HAL_OK) {
		/* Device is not connected */
		HMC5883L_lib_initialized = 0;
		return HAL_ERROR;
	}	
	
	// write CONFIG register
	HMC5883L_Set_Configurarion(HMC5883L_8SAMP, HMC5883L_RATE_15, HMC5883L_BIAS_NORMAL, HMC5883L_GAIN_1024);

	// write MODE register
	//HMC5883L_SetMode(HMC5883L_MODE_SINGLE);
	HMC5883L_SetMode(HMC5883L_MODE_CONT);	
;
	
	return HAL_OK;
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
HAL_StatusTypeDef HMC5883L_TestConnection(void)
{
	uint8_t tmp[3] = { 0 };
  HAL_StatusTypeDef res; 	
	 
	/* Test if device is connected */
	if (I2C_IsDeviceConnected(&hi2c1, HMC5883L_I2C_ADDRESS) != HAL_OK) {
		/* Device is not connected */
		return HAL_ERROR;
	}

	res = I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, HMC5883L_RA_ID_A, tmp, 3);
	
	if (res != HAL_OK){
		return HAL_ERROR;
	}
	
	if (tmp[0] == 'H' && tmp[1] == '4' && tmp[2] == '3') return HAL_OK;
	
	return HAL_ERROR;
}

// CONFIG_A register
HMC5883L_AvgSamples_TypeDef HMC5883L_GetSampleAveraging(void)
{
	uint8_t res;
	res = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A); 
	res >>= HMC5883L_CRA_AVERAGE_SHIFT;
	return (HMC5883L_AvgSamples_TypeDef) (res & HMC5883L_CRA_AVERAGE_MASK);
}

uint8_t ShiftAndAdd (uint8_t start, uint8_t val, uint8_t mask, uint8_t shift){
	return (start & ~(mask << shift)) | ((val & mask) << shift);
}

void HMC5883L_SetSampleAveraging(HMC5883L_AvgSamples_TypeDef averaging){
	uint8_t tmp;
	tmp = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A);
  tmp = ShiftAndAdd(tmp, averaging, HMC5883L_CRA_AVERAGE_MASK, HMC5883L_CRA_AVERAGE_SHIFT);
	HMC5883L_ByteWrite(HMC5883L_RA_CONFIG_A, tmp);
}

HMC5883L_DataRate_TypeDef HMC5883L_GetDataRate(void){
	uint8_t res;
	res = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A); 
	res >>= HMC5883L_CRA_RATE_SHIFT;
	return (HMC5883L_DataRate_TypeDef) (res & HMC5883L_CRA_RATE_MASK);
}

void HMC5883L_SetDataRate(HMC5883L_DataRate_TypeDef rate){
	uint8_t tmp;
	tmp = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A); 
  tmp = ShiftAndAdd(tmp, rate, HMC5883L_CRA_RATE_MASK, HMC5883L_CRA_RATE_SHIFT);
  HMC5883L_ByteWrite(HMC5883L_RA_CONFIG_A, tmp);
}

HMC5883L_BIAS_TypeDef HMC5883L_GetMeasurementBias(void)
{
	uint8_t res;
	res = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A); 
	return (HMC5883L_BIAS_TypeDef) (res & HMC5883L_CRA_BIAS_MASK);
}

void HMC5883L_SetMeasurementBias(HMC5883L_BIAS_TypeDef bias){
	uint8_t tmp;
	tmp = HMC5883L_ByteRead(HMC5883L_RA_CONFIG_A); 
  tmp = ShiftAndAdd(tmp, bias, HMC5883L_CRA_BIAS_MASK, 0);
  HMC5883L_ByteWrite(HMC5883L_RA_CONFIG_A, tmp);
}

// CONFIG_B register
HMC5883L_GainSettings_TypeDef HMC5883L_GetGain(void)
{
	uint8_t res;
	res = HMC5883L_ByteRead(HMC5883L_RA_MODE); 
	res >>= HMC5883L_CRB_GAIN_SHIFT;
	return (HMC5883L_GainSettings_TypeDef) (res & HMC5883L_CRB_GAIN_MASK);
}

void HMC5883L_SetGain(HMC5883L_GainSettings_TypeDef gain)
{
    uint8_t tmp = (gain & HMC5883L_CRB_GAIN_MASK) << HMC5883L_CRB_GAIN_SHIFT;
    HMC5883L_ByteWrite(HMC5883L_RA_CONFIG_B, tmp);
}


void HMC5883L_Set_Configurarion(HMC5883L_AvgSamples_TypeDef sample, HMC5883L_DataRate_TypeDef rate, HMC5883L_BIAS_TypeDef bias, HMC5883L_GainSettings_TypeDef gain)
{
	uint8_t tmp = 0;
	tmp  = (sample & HMC5883L_CRA_AVERAGE_MASK) << HMC5883L_CRA_AVERAGE_MASK;
	tmp |= (rate & HMC5883L_CRA_RATE_MASK) << HMC5883L_CRA_RATE_SHIFT;
	tmp |= (bias & HMC5883L_CRA_BIAS_MASK);
	HMC5883L_ByteWrite(HMC5883L_RA_CONFIG_A, tmp);
	HMC5883L_SetGain(gain);
}

// MODE register
HMC5883L_Mode_TypeDef HMC5883L_GetMode(void)
{
	uint8_t res;
	res = HMC5883L_ByteRead(HMC5883L_RA_MODE);
	return (HMC5883L_Mode_TypeDef) (res & HMC5883L_MODEREG_MASK);
}

void HMC5883L_SetMode(HMC5883L_Mode_TypeDef mode)
{
	uint8_t tmp = 0;
	tmp = mode & HMC5883L_MODEREG_MASK;
	HMC5883Lmode = mode;
	HMC5883L_ByteWrite(HMC5883L_RA_MODE, tmp);
}


uint8_t HMC5883L_GetLockStatus(void)
{
  uint8_t tmp;
 	tmp = HMC5883L_ByteRead(HMC5883L_RA_STATUS); 
	tmp >>= HMC5883L_STATUS_LOCK_BIT;
  return tmp & 0x01;
}

uint8_t HMC5883L_GetReadyStatus(void)
{
  uint8_t tmp;
 	tmp = HMC5883L_ByteRead(HMC5883L_RA_STATUS); 
	tmp >>= HMC5883L_STATUS_READY_BIT;   
  return tmp & 0x01;
}

int16_t HMC5883L_GetX(void)
{
	uint8_t i2cbuf[2];
	
	if (HMC5883L_lib_initialized == 0){
		return 0;
	}
	
  if (HMC5883Lmode == HMC5883L_MODE_SINGLE)
      HMC5883L_ByteWrite(HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE);	
	
	I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, HMC5883L_RA_DATAX_H, i2cbuf, 2);
	
	return (i2cbuf[0] << 8)| i2cbuf[1];;
}

int16_t HMC5883L_GetY(void)
{
	uint8_t i2cbuf[2];
	
	if (HMC5883L_lib_initialized == 0){
		return 0;
	}
	
  if (HMC5883Lmode == HMC5883L_MODE_SINGLE)
      HMC5883L_ByteWrite(HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE);	
	
	I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, HMC5883L_RA_DATAY_H, i2cbuf, 2);

	return (i2cbuf[0] << 8)| i2cbuf[1];
}

int16_t HMC5883L_GetZ(void)
{
	uint8_t i2cbuf[2];

	if (HMC5883L_lib_initialized == 0){
		return 0;
	}
  if (HMC5883Lmode == HMC5883L_MODE_SINGLE)
      HMC5883L_ByteWrite(HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE);	
	
	I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, HMC5883L_RA_DATAZ_H, i2cbuf, 2);
	
	return (i2cbuf[0] << 8)| i2cbuf[1];
}

void HMC5883L_GetXYZ(HMC5883L_XYZ_StructTypeDef *res)
{
	uint8_t i2cbuf[6];
	
	if (HMC5883L_lib_initialized == 0){
		return;
	}

  if (HMC5883Lmode == HMC5883L_MODE_SINGLE)
      HMC5883L_ByteWrite(HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE);	
	
	I2C_ReadBuf(&hi2c1, HMC5883L_I2C_ADDRESS, HMC5883L_RA_DATAX_H, i2cbuf, 6);
	
	res->X = (i2cbuf[0] << 8)| i2cbuf[1];
	res->Z = (i2cbuf[2] << 8)| i2cbuf[3];
	res->Y = (i2cbuf[4] << 8)| i2cbuf[5];

}

/**
 * @brief Reads the current direction angle.
 * @return Direction angle (0 or 360 means north, 180 means south
 * 90 east and 270 west).
 */
double HMC5883L_CountAngle(HMC5883L_XYZ_StructTypeDef *res) {

  double direction; // the direction angle

  // These formulas are taken from AN-203 application note
  // for the Honeywell compass
  // If the compass is horizontal these formulas actually
  // work.
  if (res->Y > 0) {
    direction = 90.0 - atan((double)res->X/(double)res->Y)*180.0/3.14159;
  } else if (res->Y < 0){
    direction = 270.0 - atan((double)res->X/(double)res->Y)*180.0/3.14159;
  } else if (res->Y == 0 && res->X < 0) {
    direction = 180.0;
  } else {
    direction = 0.0;
  }

  return direction;

}


