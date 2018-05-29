#include "mpu6050_dmp.h" 
#include "mpu6050_dump.h" 

uint8_t dmpPacketSize;

uint8_t MPU6050_DMP_Init(){
		int8_t xgOffset;
		int8_t ygOffset;
		int8_t zgOffset;

		//// get MPU hardware revision
		//DEBUG_PRINTLN(F("Selecting user bank 16..."));
		MPU6050_setMemoryBank(0x10, true, true);
		//DEBUG_PRINTLN(F("Selecting memory byte 6..."));
		MPU6050_setMemoryStartAddress(0x06);
		//DEBUG_PRINTLN(F("Checking hardware revision..."));
		//uint8_t hwRevision = MPU6050_readMemoryByte();
		//DEBUG_PRINT(F("Revision @ user[16][6] = "));
		//DEBUG_PRINTLNF(hwRevision, HEX);
		//DEBUG_PRINTLN(F("Resetting memory bank selection to 0..."));
		printf("6050 hardware revision:%x %x \n\r", MPU6050_readMemoryByte(), MPU6050_getOTPBankValid());
		MPU6050_setMemoryBank(0, false, false);
		
		//DEBUG_PRINTLN(F("Reading gyro offset values..."));
		xgOffset = MPU6050_getXGyroOffset();
		ygOffset = MPU6050_getYGyroOffset();
		zgOffset = MPU6050_getZGyroOffset();
		//DEBUG_PRINT(F("X gyro offset = "));
		//DEBUG_PRINTLN(xgOffset);
		//DEBUG_PRINT(F("Y gyro offset = "));
		//DEBUG_PRINTLN(ygOffset);
		//DEBUG_PRINT(F("Z gyro offset = "));
		//DEBUG_PRINTLN(zgOffset);	
		printf("6050 offset:%d %d %d \n\r", 
			(int16_t) xgOffset,
			(int16_t) ygOffset,
			(int16_t) zgOffset);

		//DEBUG_PRINTLN(F("Enabling interrupt latch, clear on any read, AUX bypass enabled"));
		//I2Cdev::writeByte(devAddr, MPU6050_RA_INT_PIN_CFG, 0x32);
		I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x32);

		//// load DMP code into memory banks
		//DEBUG_PRINT(F("Writing DMP code to MPU memory banks ("));
		//DEBUG_PRINT(MPU6050_DMP_CODE_SIZE);
		//DEBUG_PRINTLN(F(" bytes)"));
		if (MPU6050_writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE, 0, 0, true)) {
				//DEBUG_PRINTLN(F("Success! DMP code written and verified."));
				printf("Success! DMP code written and verified.  \n\r");

				//// write DMP configuration
        //DEBUG_PRINT(F("Writing DMP configuration to MPU memory banks ("));
        //DEBUG_PRINT(MPU6050_DMP_CONFIG_SIZE);
        //DEBUG_PRINTLN(F(" bytes in config def)"));
        if (MPU6050_writeProgDMPConfigurationSet(dmpConfig, MPU6050_DMP_CONFIG_SIZE)) {
            //DEBUG_PRINTLN(F("Success! DMP configuration written and verified."));
						printf("Success! DMP configuration written and verified.  \n\r");

            //DEBUG_PRINTLN(F("Setting DMP and FIFO_OFLOW interrupts enabled..."));
            MPU6050_setIntEnabled(0x12);

            //DEBUG_PRINTLN(F("Setting sample rate to 200Hz..."));
            MPU6050_setRate(4); // 1khz / (1 + 4) = 200 Hz

            //DEBUG_PRINTLN(F("Setting clock source to Z Gyro..."));
            MPU6050_setClockSource(MPU6050_CLOCK_PLL_ZGYRO);

            //DEBUG_PRINTLN(F("Setting DLPF bandwidth to 42Hz..."));
            MPU6050_setDLPFMode(MPU6050_DLPF_BW_42);

            //DEBUG_PRINTLN(F("Setting external frame sync to TEMP_OUT_L[0]..."));
            MPU6050_setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);

            //DEBUG_PRINTLN(F("Setting gyro sensitivity to +/- 2000 deg/sec..."));
            MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

            //DEBUG_PRINTLN(F("Setting DMP configuration bytes (function unknown)..."));
            MPU6050_setDMPConfig1(0x03);
            MPU6050_setDMPConfig2(0x00);

            //DEBUG_PRINTLN(F("Clearing OTP Bank flag..."));
            MPU6050_setOTPBankValid(false);
						printf("6050 OTP banck:%x \n\r", MPU6050_getOTPBankValid());

            //DEBUG_PRINTLN(F("Setting X/Y/Z gyro offsets to previous values..."));
            MPU6050_setXGyroOffsetTC(xgOffset);
            MPU6050_setYGyroOffsetTC(ygOffset);
            MPU6050_setZGyroOffsetTC(zgOffset);

            ////DEBUG_PRINTLN(F("Setting X/Y/Z gyro user offsets to zero..."));
            ////setXGyroOffset(0);
            ////setYGyroOffset(0);
            ////setZGyroOffset(0);

            //DEBUG_PRINTLN(F("Writing final memory update 1/19 (function unknown)..."));
            uint8_t dmpUpdate[16], j;
            uint16_t pos = 0;
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Writing final memory update 2/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Resetting FIFO..."));
            MPU6050_resetFIFO();

            //DEBUG_PRINTLN(F("Reading FIFO count..."));
            uint8_t fifoCount = MPU6050_getFIFOCount();

            //DEBUG_PRINT(F("Current FIFO count="));
            //DEBUG_PRINTLN(fifoCount);
						printf("6050 fifoCount:%d \n\r", fifoCount);

            uint8_t fifoBuffer[128];
            //getFIFOBytes(fifoBuffer, fifoCount);

            //DEBUG_PRINTLN(F("Writing final memory update 3/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Writing final memory update 4/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Disabling all standby flags..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_2, 0x00);

            //DEBUG_PRINTLN(F("Setting accelerometer sensitivity to +/- 2g..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0x00);

            //DEBUG_PRINTLN(F("Setting motion detection threshold to 2..."));
            MPU6050_setMotionDetectionThreshold(2);

            //DEBUG_PRINTLN(F("Setting zero-motion detection threshold to 156..."));
            MPU6050_setZeroMotionDetectionThreshold(156);

            //DEBUG_PRINTLN(F("Setting motion detection duration to 80..."));
            MPU6050_setMotionDetectionDuration(80);

            //DEBUG_PRINTLN(F("Setting zero-motion detection duration to 0..."));
            MPU6050_setZeroMotionDetectionDuration(0);

            //DEBUG_PRINTLN(F("Setting AK8975 to single measurement mode..."));
            ////mag -> setMode(1);
            //I2Cdev::writeByte(0x0E, 0x0A, 0x01);

            //// setup AK8975 (0x0E) as Slave 0 in read mode
            //DEBUG_PRINTLN(F("Setting up AK8975 read slave 0..."));
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_ADDR, 0x8E);
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_REG,  0x01);
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_CTRL, 0xDA);

            //// setup AK8975 (0x0E) as Slave 2 in write mode
            //DEBUG_PRINTLN(F("Setting up AK8975 write slave 2..."));
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_ADDR, 0x0E);
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_REG,  0x0A);
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_CTRL, 0x81);
            //I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_DO,   0x01);

            //// setup I2C timing/delay control
            //DEBUG_PRINTLN(F("Setting up slave access delay..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_I2C_SLV4_CTRL, 0x18);
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x05);

            //// enable interrupts
            //DEBUG_PRINTLN(F("Enabling default interrupt behavior/no bypass..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);

            //// enable I2C master mode and reset DMP/FIFO
            //DEBUG_PRINTLN(F("Enabling I2C master mode..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, 0x20);
            //DEBUG_PRINTLN(F("Resetting FIFO..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, 0x24);
            //DEBUG_PRINTLN(F("Rewriting I2C master mode enabled because...I don't know"));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, 0x20);
            //DEBUG_PRINTLN(F("Enabling and resetting DMP/FIFO..."));
            I2Cdev_writeByte(MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, 0xE8);

            //DEBUG_PRINTLN(F("Writing final memory update 5/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 6/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 7/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 8/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 9/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 10/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 11/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            
            //DEBUG_PRINTLN(F("Reading final memory update 12/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
						//{
						//	dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
						//	printf("6050 dmp tst:%d %d %02x \n\r", j, pos, dmpUpdate[j]);
						//}
            MPU6050_readMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            //#ifdef DEBUG
            //    DEBUG_PRINT(F("Read bytes: "));
            //    for (j = 0; j < 4; j++) {
            //        DEBUG_PRINTF(dmpUpdate[3 + j], HEX);
            //        DEBUG_PRINT(" ");
						//				printf("6050 dmp upd:%d %02x \n\r", j, dmpUpdate[3 + j]);
            //    }
            //    DEBUG_PRINTLN("");
            //#endif

            //DEBUG_PRINTLN(F("Writing final memory update 13/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 14/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 15/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 16/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);
            //DEBUG_PRINTLN(F("Writing final memory update 17/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Waiting for FIRO count >= 46..."));
            while ((fifoCount = MPU6050_getFIFOCount()) < 46);
						printf("6050 fifoCount:%d \n\r", fifoCount);
            //DEBUG_PRINTLN(F("Reading FIFO..."));
            MPU6050_getFIFOBytes(fifoBuffer, MIN(fifoCount, 128)); // safeguard only 128 bytes
            //DEBUG_PRINTLN(F("Reading interrupt status..."));
            MPU6050_getIntStatus();

            //DEBUG_PRINTLN(F("Writing final memory update 18/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Waiting for FIRO count >= 48..."));
            while ((fifoCount = MPU6050_getFIFOCount()) < 48);
						//printf("6050 fifoCount:%d \n\r", fifoCount);

						//DEBUG_PRINTLN(F("Reading FIFO..."));
            MPU6050_getFIFOBytes(fifoBuffer, MIN(fifoCount, 128)); // safeguard only 128 bytes
            //DEBUG_PRINTLN(F("Reading interrupt status..."));
            MPU6050_getIntStatus();
            //DEBUG_PRINTLN(F("Waiting for FIRO count >= 48..."));
            while ((fifoCount = MPU6050_getFIFOCount()) < 48);
						printf("6050 fifoCount:%d \n\r", fifoCount);

						//DEBUG_PRINTLN(F("Reading FIFO..."));
            MPU6050_getFIFOBytes(fifoBuffer, MIN(fifoCount, 128)); // safeguard only 128 bytes
            //DEBUG_PRINTLN(F("Reading interrupt status..."));
            MPU6050_getIntStatus();

            //DEBUG_PRINTLN(F("Writing final memory update 19/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            MPU6050_writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1], true, false);

            //DEBUG_PRINTLN(F("Disabling DMP (you turn it on later)..."));
            MPU6050_setDMPEnabled(false);

            //DEBUG_PRINTLN(F("Setting up internal 48-byte (default) DMP packet buffer..."));
            dmpPacketSize = 48;
            /*if ((dmpPacketBuffer = (uint8_t *)malloc(42)) == 0) {
                return 3; // TODO: proper error code for no memory
            }*/

            //DEBUG_PRINTLN(F("Resetting FIFO and clearing INT status one last time..."));
            MPU6050_resetFIFO();
            MPU6050_getIntStatus();
        } else {
            //DEBUG_PRINTLN(F("ERROR! DMP configuration verification failed."));
            return 2; // configuration block loading failed
        }
    } else {
        //DEBUG_PRINTLN(F("ERROR! DMP code verification failed."));
        return 1; // main binary block loading failed
    }
    return 0; // success
	
}

uint16_t MPU6050_dmpGetFIFOPacketSize() {
    return dmpPacketSize;
}





