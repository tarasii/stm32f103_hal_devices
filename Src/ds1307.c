#include "ds1307.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t DS1307_lib_initialized = 0;

HAL_StatusTypeDef DS1307_Init(void) {
	if (I2C_IsDeviceConnected(&hi2c1, DS1307_I2C_ADDRESS) != HAL_OK) {
		/* Device is not connected */
		return HAL_ERROR;
	}	
	DS1307_lib_initialized = 1;
	return HAL_OK;
}

uint8_t* DS1307_ReadTime(void) {
    uint8_t *received_data = calloc(TIME_STRUCT_SIZE, sizeof(uint8_t));

	  I2C_ReadBuf(&hi2c1, DS1307_I2C_ADDRESS, 0, received_data, TIME_STRUCT_SIZE);

    return received_data;
}

void DS1307_DecodeTime(const uint8_t *data, DS1307_time *s_time) {
    // decode seconds
    uint8_t msd = 0, lsd = 0;
    int8_t /*am_pm = -1,*/_12h_mode = -1;


		lsd = (data[0] & 0x0f);
    msd = (data[0] & 0x70) >> 4;
    s_time->seconds = lsd + 10 * msd;

    lsd = (data[1] & 0x0f);
    msd = (data[1] & 0x70) >> 4;
    s_time->minutes = lsd + 10 * msd;

    // If 1, then 12-hour mode is enabled, 0 - 24-hour mode
    _12h_mode = (data[2] & 0x40) >> 6;

    // When 12-hour mode enabled, PM = 1, AM = 0, otherwise first bit of
    // hour_msd
    if (_12h_mode) {
        //am_pm = (data[2] & 0x20) >> 5;
        msd = (data[2] & 0x10) >> 4;
    } else {
        msd = (data[2] & 0x30) >> 4;
    }
    lsd = (data[2] & 0x0f);
    s_time->hours = lsd + 10 * msd;

    s_time->day_of_week = (data[3] & 0x07);

    lsd = (data[4] & 0x0f);
    msd = (data[4] & 0x30) >> 4;
    s_time->date = lsd + 10 * msd;

    lsd = (data[5] & 0x0f);
    msd = (data[5] & 0x10) >> 4;
    s_time->month = lsd + 10 * msd;

    lsd = (data[6] & 0x0f);
    msd = (data[6] & 0xf0) >> 4;
    s_time->year = lsd + 10 * msd;

    s_time->clock_halt = (data[0] & 0x80) >> 7;
    s_time->out = (data[7] & 0x80) >> 7;
    s_time->sqwe = (data[7] & 0x10) >> 4;
    s_time->rs1 = (data[7] & 0x02) >> 1;
    s_time->rs0 = (data[7] & 0x01);
}

// Each number is represented in BCD format, according to documentation
uint8_t* DS1307_EncodeData(const DS1307_time *s_time) {
    uint8_t *data = calloc(TIME_STRUCT_SIZE, sizeof(uint8_t));
    uint8_t msd, lsd;

    // 0x00 Clock halt and seconds
    msd = s_time->seconds / 10;
    lsd = s_time->seconds - msd * 10;
    data[0] = (s_time->clock_halt << 7) | (msd << 4) | (lsd);

    // 0x01 Minutes
    msd = s_time->minutes / 10;
    lsd = s_time->minutes - msd * 10;
    data[1] = (msd << 4) | (lsd);

    // 0x02 Hours
    msd = s_time->hours / 10;
    lsd = s_time->hours - msd * 10;
    data[2] = (0 << 6 /*24h mode*/) | (msd << 4) | (lsd);

    // 0x03 Day of week
    data[3] = s_time->day_of_week;

    // 0x04 Date (day of month)
    msd = s_time->date / 10;
    lsd = s_time->date - msd * 10;
    data[4] = (msd << 4) | (lsd);

    // 0x05 Month
    msd = s_time->month / 10;
    lsd = s_time->month - msd * 10;
    data[5] = (msd << 4) | (lsd);

    // 0x06 Year
    msd = s_time->year / 10;
    lsd = s_time->year - msd * 10;
    data[6] = (msd << 4) | (lsd);

    // 0x07 Control part:
    // OUT, SQWE, RS1 and RS0
    data[7] = (s_time->out << 7) | (s_time->sqwe << 4) | (s_time->rs1 << 1)
            | (s_time->rs0);

    return data;
}

void DS1307_WriteTime(const DS1307_time *s_time) {
    uint8_t *data = DS1307_EncodeData(s_time);
    //printRawData(data, TIME_STRUCT_SIZE);

	  I2C_WriteBuf(&hi2c1, DS1307_I2C_ADDRESS, 0, data, TIME_STRUCT_SIZE);

    free(data);
}

void DS1307_PrintTime(const DS1307_time *s_time) {
    printf("%2d:%2d:%2d\n\r", s_time->hours, s_time->minutes, s_time->seconds);

    switch (s_time->day_of_week) {
    case 1:
        printf("Monday\n\r");
        break;
    case 2:
        printf("Tuesday\n\r");
        break;
    case 3:
        printf("Wednesday\n\r");
        break;
    case 4:
        printf("Thursday\n\r");
        break;
    case 5:
        printf("Friday\n\r");
        break;
    case 6:
        printf("Saturday\n\r");
        break;
    case 7:
        printf("Sunday\n\r");
        break;

    default:
        printf("BORKEN\n\r");
        break;
    }

    printf("%2d.%2d.%2d\n\r", s_time->date, s_time->month, s_time->year);
    printf("Config: CH: %d OUT: %d SQWE: %d RS1: %d RS0: %d\n\r",
            s_time->clock_halt, s_time->out, s_time->sqwe, s_time->rs1,
            s_time->rs0);
}

//void printByte(const uint8_t n) {
//    unsigned i;
//    for (i = 1 << 7; i > 0; i = i / 2)
//        (n & i) ? printf("1") : printf("0");
//    printf("\n\r");
//}

//void printRawData(const uint8_t *data, const uint8_t size) {
//    if (data == 0) {
//        return;
//    }

//    uint8_t idx;

//    for (idx = 0; idx < size; ++idx) {
//        printByte(data[idx]);
//    }
//}

DS1307_time DS1307_GetTime(void) {
    uint8_t* data = DS1307_ReadTime();
    DS1307_time s_time;
    DS1307_DecodeTime(data, &s_time);
    free(data);
    return s_time;
}

inline bool DS1307_CheckAddreses(const uint8_t start_address, const uint8_t allocated_bytes) {
    if ((allocated_bytes <= 0) || (start_address < 0)
            || (start_address + allocated_bytes > DS1307_RAM_SIZE)) {
        return false;
    }
    return true;
}

uint8_t* DS1307_Read(const uint8_t start_address, const uint8_t allocated_bytes) {
    if (!DS1307_CheckAddreses(start_address, allocated_bytes)) {
        return 0;
    }

    uint8_t *received_data = calloc(allocated_bytes, sizeof(uint8_t));
    uint8_t index = start_address;
		
		I2C_ReadBuf(&hi2c1, DS1307_I2C_ADDRESS << 1, index, received_data, allocated_bytes);

    return received_data;
}
