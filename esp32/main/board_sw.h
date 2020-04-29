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

#ifndef ESP32_MAIN_BOARD_SW_H_
#define ESP32_MAIN_BOARD_SW_H_

#include <stdint.h>
#include <hal.h>
#include <board_dev.h>
#include <drv_i2c_tca9548a.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup board_sw Board I2C Switch Control
 * @ingroup board
 * @brief
 * @{
 */

typedef struct board_dev_sw_t {
  hal_i2c_dev_t i2c_dev;  //!< I2C device to use
  board_dev_status_t status;
} board_dev_sw_t;

/**
 * @brief
 *
 * @param sw
 * @param i2c_dev
 */
void sw_init(board_dev_sw_t* sw, hal_i2c_dev_t i2c_dev);

/**
 * @brief
 *
 * @param sw
 * @param ch
 */
board_dev_status_t sw_set_channel(board_dev_sw_t* sw, uint8_t ch);

/**
 * @brief
 *
 * @param sw
 * @param ch
 * @return board_dev_status_t
 */
board_dev_status_t sw_get_channel(board_dev_sw_t* sw, uint8_t* ch);

/**
 * @brief
 *
 * @param sw
 * @return board_dev_status_t
 */
board_dev_status_t sw_get_status(const board_dev_sw_t* sw);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_BOARD_SW_H_
