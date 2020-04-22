/*
Copyright 2020 TRIUMF

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ESP32_MAIN_HAL_INTERFACE_H_
#define ESP32_MAIN_HAL_INTERFACE_H_

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup hal HAL
 * @brief Hardware Abstraction for I2C, GPIO, etc
 * @{
 */

#define HAL_LOG_MAX_LEN 80

/** SCL I2C pin */
#define HAL_I2C_MASTER_SCL_IO_PIN 22

/** SDA I2C pin */
#define HAL_I2C_MASTER_SDA_IO_PIN 23

/** 400kHz I2C bus master */
#define HAL_I2C_MASTER_FREQ 400000

/** I2C is in APB 80MHz clock period */
#define HAL_I2C_TIMEOUT_PERIOD_IN_US(x) ((x) * (I2C_APB_CLK_FREQ / 1000000))

/** Default I2C timeout period to use */
#define HAL_I2C_DEFAULT_TIMEOUT_PERIOD HAL_I2C_TIMEOUT_PERIOD_IN_US(10)

#define HAL_I2C_PS1_ADDR MS5525DSO_I2C_ADDR_HIGH
#define HAL_I2C_FS1_ADDR SFM3000_I2C_ADDR
#define HAL_I2C_SWITCH_ADDR TCA9548A_ADDR_LLL

#define HAL_GPIO_DRV_RSTn_PIN 14
#define HAL_GPIO_LED1_PIN 13
#define HAL_GPIO_LED2_PIN 12
#define HAL_GPIO_DRV_CH4_PIN 32
#define HAL_GPIO_DRV_CH3_PIN 15
#define HAL_GPIO_DRV_CH2_PIN 33
#define HAL_GPIO_DRV_CH1_PIN 27

#define HAL_I2C_SWITCH_CH_PS1 (1 << 7)
#define HAL_I2C_SWITCH_CH_FS1 (1 << 0)

typedef int64_t hal_timestamp_t;

typedef enum hal_i2c_dev_t {
    HAL_I2C_DEV_SWITCH,
    HAL_I2C_DEV_PS1,
    HAL_I2C_DEV_FS1,
} hal_i2c_dev_t;

typedef enum hal_log_level_t {
  HAL_LOG_NONE,
  HAL_LOG_ERROR,
  HAL_LOG_WARN,
  HAL_LOG_INFO,
  HAL_LOG_DEBUG
} hal_log_level_t;

/**
 * @brief HAL errors
 *
 */
typedef enum hal_err_t { HAL_OK, HAL_ERR_FAIL } hal_err_t;


typedef gpio_num_t hal_gpio_t;

/**
 * @brief I2C configuration for ESP32
 *
 */
typedef struct hal_i2c_config_t {
  uint8_t i2c_addr;         //!< I2C address of device
  i2c_port_t i2c_port_num;  //!< I2C master port to use on ESP32
  TickType_t i2c_timeout;   //!< I2C timeout in ticks
} hal_i2c_config_t;


void hal_init(void);

/**
 * @brief
 *
 * @param pin
 * @param value
 */
void hal_gpio_write(uint32_t pin, int value);

/**
 * @brief
 *
 * @param pin
 * @return int
 */
int hal_gpio_read(hal_gpio_t pin);

/**
 * @brief Set the current log level
 *
 * @param new_log_level New log level to use
 */
void hal_set_log_level(hal_log_level_t new_log_level);

/**
 * @brief Get the current log level
 *
 * @return hal_log_level_t
 */
hal_log_level_t hal_get_log_level(void);

/**
 * @brief Log a message to the system
 *
 * @param log_level
 * @param topic
 * @param fmt
 * @param ... Variadic
 */
void hal_log(hal_log_level_t log_level, const char* topic, const char* fmt,
             ...);


hal_timestamp_t hal_get_timestamp(void);

/**
 * @brief Get the I2C configuration of a given board device
 *
 * @param dev Board I2C device to get configuration of
 * @return const hal_i2c_config_t*
 */
const hal_i2c_config_t* hal_i2c_get_config(hal_i2c_dev_t dev);

/**
 * @brief Writes data to given I2C device
 *
 * @param cfg I2C configuration of device
 * @param buffer Buffer of byte to write to the i2c device
 * @param len Number of bytes to write, must not exceed length of buffer
 * @return hal_err_t
 */
hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len);  //!< Function pointer to i2c_write

/**
 * @brief Reads data from given I2C device
 *
 * @param cfg I2C configuration of device
 * @param buffer Buffer of byte to store read from i2c device
 * @param len Number of bytes to read, must not exceed length of buffer
 * @return hal_err_t
 */
hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len);  //!< Function pointer to i2c_read

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_HAL_INTERFACE_H_
