#include "aht20.h"
#include "i2c.h"

#define AHT20_ADDRESS 0x70

/**
 * @brief 初始化AHT20
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
 * @brief AHT20重置寄存器
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
 * @brief 获取温度和湿度
 * 
 * @param Temperature 用于接收温度的变量地址
 * @param Humidity 用于接收湿度的变量地址
 * 
 * @note 建议获取间隔大于500毫秒
*/
void AHT20_Read(float *Temperature, float *Humidity) {
	uint8_t cmd_ac[3] = { 0xAC, 0x33, 0x00 };
	uint8_t read_buf[6] = { 0 };
	int32_t data = 0;
	int16_t hum = 0, tmp = 0;
	HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, cmd_ac, 3, 10);
	HAL_Delay(200);
	HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, read_buf, 6, 10);
	if ((read_buf[0] & 0x80) != 0x80) {
		data = (data | read_buf[1]) << 8;
		data = (data | read_buf[2]) << 8;
		data = (data | read_buf[3]);
		data = data >> 4;
		hum = data * 100 * 10 / 1024 / 1024;
		data = 0;
		data = (data | read_buf[3]) << 8;
		data = (data | read_buf[4]) << 8;
		data = (data | read_buf[5]);
		data = data & 0xFFFFF;
		tmp = data * 200 * 10 / 1024 / 1024 - 500;
		*Temperature = ((float) tmp) / 10.0f;
		*Humidity = ((float) hum) / 10.0f;
	}
}

