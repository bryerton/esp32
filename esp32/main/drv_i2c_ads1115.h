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
along with this program.  If not, see www.gnu.org/licenses/.
*/

#ifndef ESP32_MAIN_DRV_I2C_ADS1115_H_
#define ESP32_MAIN_DRV_I2C_ADS1115_H_

#include <hal.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ads1115 ADS1115
 * @ingroup driver_i2c
 * @brief I2C Driver for the Texas Instruments, 860-SPS, 16-Bit ADC with
 * Programmable Comparator
 * @{
 */

/** Conversion register */
#define ADS1115_REG_CONVERSION 0x00u

/** Configuration register */
#define ADS1115_REG_CONFIG 0x01u

/** Lo_thresh register */
#define ADS1115_REG_LO_THRESH 0x02u

/** Hi_thresh register */
#define ADS1115_REG_HI_THRESH 0x03u

typedef struct ads1115_config_t {
  uint8_t one_shot;
  uint8_t mux;
  uint8_t pga;
  uint8_t mode;
  uint8_t data_rate;
  uint8_t comp_mode;
  uint8_t comp_pol;
  uint8_t comp_lat;
  uint8_t comp_que;
} ads1115_config_t;

/**
 * @brief
 *
 * @param cfg
 * @param value
 * @return hal_err_t
 */
hal_err_t ads1115_read_conversion(hal_i2c_config_t* cfg, int16_t* value);

/**
 * @brief
 *
 * @param cfg
 * @param config
 * @return hal_err_t
 */
hal_err_t ads1115_write_config(hal_i2c_config_t* cfg, ads1115_config_t* config);

/**
 * @brief
 *
 * @param cfg
 * @param config
 * @return hal_err_t
 */
hal_err_t ads1115_read_config(hal_i2c_config_t* cfg, ads1115_config_t* config);

/**
 * @brief
 *
 * @param cfg
 * @param value
 * @return hal_err_t
 */
hal_err_t ads1115_write_hi_thresh(hal_i2c_config_t* cfg, int16_t* value);

/**
 * @brief
 *
 * @param cfg
 * @param value
 * @return hal_err_t
 */
hal_err_t ads1115_read_hi_thresh(hal_i2c_config_t* cfg, int16_t* value);

/**
 * @brief
 *
 * @param cfg
 * @param value
 * @return hal_err_t
 */
hal_err_t ads1115_write_lo_thresh(hal_i2c_config_t* cfg, int16_t* value);

/**
 * @brief
 *
 * @param cfg
 * @param value
 * @return hal_err_t
 */
hal_err_t ads1115_read_lo_thresh(hal_i2c_config_t* cfg, int16_t* value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_DRV_I2C_ADS1115_H_
