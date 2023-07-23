#include "aht20.h"
#include "i2c.h"

#define AHT20_ADDRESS 0x70

#define READ_DECIMAL_SHIFT 12		// 20 max
#define READ_DECIMAL_SCALE 4096

/**
 * @brief Initialize AHT20
 * 
*/
void AHT20_Init(void) {
	uint8_t read_buf = 0;
	HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &read_buf, 1, 10);
	if ((read_buf & 0x18) != 0x18) {
		AHT20_ResetRegister(0x1B);
		AHT20_ResetRegister(0x1C);
		AHT20_ResetRegister(0x1E);
	}
}

/**
 * @brief AHT20_ResetRegister
 * 
*/
void AHT20_ResetRegister(uint8_t addr) {
	uint8_t send_buf[3] = { 0x00 };
	send_buf[0] = addr;
	HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, send_buf, 3, 10);
	HAL_Delay(5);
	HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, send_buf, 3, 10);
	HAL_Delay(10);
	send_buf[0] = 0xB0 | addr;
	HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, send_buf, 3, 10);
}

/**
 * @brief Start measuring (requires 80ms to complete for reading)
 * 
*/
void AHT20_Measure() {
	uint8_t cmd_ac[3] = { 0xAC, 0x33, 0x00 };
	HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, cmd_ac, 3, 10);
}

uint8_t AHT20_CalcCRC8(uint8_t *data, uint8_t length)
{
	uint8_t crc = 0xFF;
	for (uint8_t byte = 0; byte < length; byte++)
	{
		crc ^= (data[byte]);
		for(uint8_t i = 8; i > 0; --i)
		{
			if(crc & 0x80)
				crc = (crc << 1) ^ 0x31;
			else
				crc = (crc << 1);
		}
	}
	return crc;
}

/**
 * @brief Read temperature and humidity
 * 
 * @param temperature A float pointer to write temperature result
 * @param humidity  A float pointer to write humidity result
 * 
 * @return whether success or not
*/
uint8_t AHT20_Read(float *temperature, float *humidity) {
	uint8_t readBuf[7] = { 0 };
	HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, readBuf, 7, 10);
	if ((readBuf[0] & 0x80) == 0x80) {
		// Not ready
		return 0;
	}
	uint8_t crc = AHT20_CalcCRC8(readBuf, 6);
	if (crc != readBuf[6]) {
		// CRC failed
		return 0;
	}
	int64_t rawHum = readBuf[1] << 12 | readBuf[2] << 4 | readBuf[3] >> 4;
	int32_t hum = ((rawHum << READ_DECIMAL_SHIFT) * 100) >> 20;
	int64_t rawTmp = (readBuf[3] & 0xf) << 16 | readBuf[4] << 8 | readBuf[5];
	int32_t tmp = (((rawTmp << READ_DECIMAL_SHIFT) * 200) >> 20) - (50 << READ_DECIMAL_SHIFT);
	*temperature = ((float) tmp) / READ_DECIMAL_SCALE;
	*humidity = ((float) hum) / READ_DECIMAL_SCALE;
	return 1;
}
