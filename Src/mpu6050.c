#include "mpu6050.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t MPU6050_lib_initialized = 0;

uint8_t MPU6050_ByteRead(uint8_t slaveAddr, uint8_t adress) {
	if (MPU6050_lib_initialized == 0){
		return 0;
	}
  uint8_t received_data;
  I2C_ReadBuf(&hi2c1, slaveAddr, adress, &received_data, 1);
  return received_data;
}
	
void MPU6050_ByteWrite(uint8_t slaveAddr, uint8_t adress, uint8_t data) {
	if (MPU6050_lib_initialized == 0){
		return;
	}
  I2C_WriteBuf(&hi2c1, slaveAddr, adress, &data, 1);
}

/** Write multiple bits in an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 */
void MPU6050_WriteBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t tmp;
    //MPU6050_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
		tmp = MPU6050_ByteRead(slaveAddr, regAddr);
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    tmp &= ~(mask); // zero all important bits in existing byte
    tmp |= data; // combine data with existing byte
    MPU6050_ByteWrite(slaveAddr, regAddr, tmp);
}

/** write a single bit in an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 */
void MPU6050_WriteBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    //MPU6050_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
		tmp = MPU6050_ByteRead(slaveAddr, regAddr);
		tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    MPU6050_ByteWrite(slaveAddr, regAddr, tmp);
}

/** Read multiple bits from an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */
void MPU6050_ReadBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t tmp;
    //MPU6050_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
		tmp = MPU6050_ByteRead(MPU6050_ADDRESS_AD0_LOW, regAddr);
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    tmp &= mask;
    tmp >>= (bitStart - length + 1);
    *data = tmp;
}

/** Read a single bit from an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */
void MPU6050_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data)
{
	uint8_t tmp;
	//MPU6050_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
	tmp = MPU6050_ByteRead(slaveAddr, regAddr);
	*data = tmp & (1 << bitNum);
}

HAL_StatusTypeDef MPU6050_Initialize(void)
{
	MPU6050_lib_initialized = 1;
	
	if (MPU6050_TestConnection() != HAL_OK) {
		/* Device is not connected */
		MPU6050_lib_initialized = 0;
		return HAL_ERROR;
	}
	
	MPU6050_SetClockSource(MPU6050_CLOCK_PLL_XGYRO);
  MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
  MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  MPU6050_SetSleepModeStatus(DISABLE);
	
  return HAL_OK;
}


HAL_StatusTypeDef MPU6050_TestConnection(void)
{
  HAL_StatusTypeDef res;
	uint8_t tmp = 0;
	
	/* Test if device is connected */
	if (I2C_IsDeviceConnected(&hi2c1, MPU6050_I2C_ADDRESS) != HAL_OK) {
		/* Device is not connected */
		return HAL_ERROR;
	}

	res = I2C_ReadBuf(&hi2c1, MPU6050_I2C_ADDRESS, MPU6050_RA_WHO_AM_I, &tmp, 1);
	
	if (res != HAL_OK){
		return HAL_ERROR;
	}
	
	if (tmp == 0x68) return HAL_OK;
	
	return HAL_ERROR;    
	//return MPU6050_GetDeviceID() == 0x34 ? 1 : 0; //0b110100; 8-bit representation in hex = 0x34
}


uint8_t MPU6050_GetDeviceID(void)
{
    uint8_t tmp;
    MPU6050_ReadBits(MPU6050_I2C_ADDRESS, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, &tmp);
    return tmp;
}

/** Set clock source setting.
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see MPU6050_GetClockSource()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLKSEL_BIT
 * @see MPU6050_PWR1_CLKSEL_LENGTH
 */
void MPU6050_SetClockSource(uint8_t source)
{
    MPU6050_WriteBits(MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see MPU6050_GetFullScaleGyroRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_SetFullScaleGyroRange(uint8_t range)
{
    MPU6050_WriteBits(MPU6050_I2C_ADDRESS, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

// GYRO_CONFIG register

/** Get full-scale gyroscope range.
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below.
 *
 * <pre>
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 * </pre>
 *
 * @return Current full-scale gyroscope range setting
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
uint8_t MPU6050_GetFullScaleGyroRange(void)
{
    uint8_t tmp;
    MPU6050_ReadBits(MPU6050_I2C_ADDRESS, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, &tmp);
    return tmp;
}

/** Get full-scale accelerometer range.
 * The FS_SEL parameter allows setting the full-scale range of the accelerometer
 * sensors, as described in the table below.
 *
 * <pre>
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 * </pre>
 *
 * @return Current full-scale accelerometer range setting
 * @see MPU6050_ACCEL_FS_2
 * @see MPU6050_RA_ACCEL_CONFIG
 * @see MPU6050_ACONFIG_AFS_SEL_BIT
 * @see MPU6050_ACONFIG_AFS_SEL_LENGTH
 */
uint8_t MPU6050_GetFullScaleAccelRange(void)
{
    uint8_t tmp;
    MPU6050_ReadBits(MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, &tmp);
    return tmp;
}

/** Set full-scale accelerometer range.
 * @param range New full-scale accelerometer range setting
 * @see MPU6050_GetFullScaleAccelRange()
 */
void MPU6050_SetFullScaleAccelRange(uint8_t range)
{
    MPU6050_WriteBits(MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/** Get sleep mode status.
 * Setting the SLEEP bit in the register puts the device into very low power
 * sleep mode. In this mode, only the serial interface and internal registers
 * remain active, allowing for a very low standby current. Clearing this bit
 * puts the device back into normal mode. To save power, the individual standby
 * selections for each of the gyros should be used if any gyro axis is not used
 * by the application.
 * @return Current sleep mode enabled status
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
bool MPU6050_GetSleepModeStatus(void)
{
    uint8_t tmp;
    MPU6050_ReadBit(MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, &tmp);
    return tmp == 0x00 ? false : true;
}

/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see MPU6050_GetSleepModeStatus()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
void MPU6050_SetSleepModeStatus(FunctionalState NewState)
{
    MPU6050_WriteBit(MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, NewState);
}

/** Get raw 6-axis motion sensor readings (accel/gyro).
 * Retrieves all currently available motion sensor values.
 * @param AccelGyro 16-bit signed integer array of length 6
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
void MPU6050_GetRawAccelGyro(int16_t* AccelGyro)
{
    uint8_t tmpBuffer[14];
    //MPU6050_I2C_BufferRead(MPU6050_I2C_ADDRESS, tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 14);
    I2C_ReadBuf(&hi2c1, MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, tmpBuffer, 14);
  	/* Get acceleration */
    for (int i = 0; i < 3; i++)
        AccelGyro[i] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
    /* Get Angular rate */
    for (int i = 4; i < 7; i++)
        AccelGyro[i - 1] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);

}

int16_t MPU6050_GetRawTemperature(void)
{
    uint8_t tmpBuffer[2];
    //MPU6050_I2C_BufferRead(MPU6050_I2C_ADDRESS, tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 14);
    I2C_ReadBuf(&hi2c1, MPU6050_I2C_ADDRESS, MPU6050_RA_TEMP_OUT_H, tmpBuffer, 2);
  	/* Get acceleration */
    return ((int16_t) ((uint16_t) tmpBuffer[0] << 8) + tmpBuffer[1]);

}

float MPU6050_GetTemperature(int16_t temperature)
{
    return ((float) temperature / 340 + 36.53);
}

