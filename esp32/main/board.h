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

#ifndef ESP32_MAIN_BOARD_H_
#define ESP32_MAIN_BOARD_H_

#include <hal.h>
#include <stdint.h>
#include <board_dev.h>
#include <board_sw.h>
#include <board_ps.h>
#include <board_fs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup board Board
 * @brief Control board support package for the ESP32
 * @{
 */

/** Wait in reset for 100ms */
#define BOARD_HARD_RESET_TIME 500000

/** Wait for a maximum of 500ms for the i2c devices to reset */
#define BOARD_SOFT_RESET_TIMEOUT 2000000

typedef enum board_state_t {
  BOARD_ST_HARD_RESET,
  BOARD_ST_HARD_RESET_WAIT,
  BOARD_ST_SOFT_RESET,
  BOARD_ST_SOFT_RESET_WAIT,
  BOARD_ST_RUNNING,
} board_state_t;

typedef struct board_t {
  board_state_t state;
  board_dev_sw_t sw;
  board_dev_ps_t ps1;
  board_dev_fs_t fs1;
  hal_timestamp_t ts_state;
} board_t;

/**
 * @brief Initialize board
 *
 * Attempt to initialize all the drivers, read coefficients, start timers etc.
 *
 * @param board
 */
void board_init(board_t* board);

/**
 * @brief Update the board
 *
 * Queries the drivers and updates the status and values for readback
 *
 * @param board
 */
void board_update(board_t* board);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_BOARD_H_
