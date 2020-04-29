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

#ifndef ESP32_MAIN_BOARD_FS_H_
#define ESP32_MAIN_BOARD_FS_H_

#include <stdint.h>
#include <hal.h>
#include <board_dev.h>
#include <drv_i2c_sfm3000.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup board_fs Board SFM3000 Control
 * @ingroup board
 * @brief
 * @{
 */

/** Wait for reset to clear for 200ms */
#define BOARD_FS_RESET_TIME 200000

/** Wait for 2ms for conversion */
#define BOARD_FS_CONVERSION_TIME 1000

typedef enum flow_sensor_state_t {
  FS_SENSOR_ST_RESET,
  FS_SENSOR_ST_CONFIG,
  FS_SENSOR_ST_DISCARD_FIRST_FLOW,
  FS_SENSOR_ST_READ_FLOW,
} flow_sensor_state_t;

typedef struct board_dev_fs_t {
  hal_i2c_dev_t i2c_dev;  //!< I2C device to use
  board_dev_status_t status;
  hal_timestamp_t ts_state;
  flow_sensor_state_t state;
  uint32_t product;
  uint32_t serial;
  uint16_t flow_raw;
  float flow;
  sfm3000_settings_t settings;
} board_dev_fs_t;

/**
 * @brief
 *
 * @param fs
 * @param i2c_dev
 * @param settings
 */
void fs_init(board_dev_fs_t* fs, hal_i2c_dev_t i2c_dev, const sfm3000_settings_t* settings);

/**
 * @brief
 *
 * @param fs
 * @return board_dev_status_t
 */
board_dev_status_t fs_update(board_dev_fs_t* fs);

/**
 * @brief
 *
 * @param fs
 * @param settings
 */
void fs_set_settings(board_dev_fs_t* fs, const sfm3000_settings_t* settings);

/**
 * @brief
 *
 * @param fs
 * @param flow
 * @return board_dev_status_t
 */
board_dev_status_t fs_get_flow(board_dev_fs_t* fs, float* flow);

/**
 * @brief
 *
 * @param fs
 * @return board_dev_status_t
 */
board_dev_status_t fs_get_status(board_dev_fs_t* fs);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_BOARD_FS_H_
