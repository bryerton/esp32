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

#ifndef ESP32_MAIN_BOARD_PS_H_
#define ESP32_MAIN_BOARD_PS_H_

#include <stdint.h>
#include <hal.h>
#include <board.h>
#include <board_dev.h>
#include <drv_i2c_ms5525dso.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup board_ps Board Pressure Sensor Control
 * @ingroup board
 * @brief
 * @{
 */

/** Wait for reset to clear for 100ms */
#define BOARD_PS_RESET_TIME 100000

/** Wait 2ms for conversion to finish */
#define BOARD_PS_CONVERSION_TIME 2000

typedef enum pressor_sensor_state_t {
  PS_SENSOR_ST_ERR,
  PS_SENSOR_ST_RESET,
  PS_SENSOR_ST_CONFIG,
  PS_SENSOR_ST_READ_CH1,
  PS_SENSOR_ST_READ_CH2,
} pressor_sensor_state_t;

typedef struct board_dev_ps_t {
  hal_timestamp_t ts_state;  //!< Timestamp of when current state was entered
  hal_i2c_dev_t i2c_dev;     //!< I2C device to use
  board_dev_status_t status;
  uint32_t d1;
  uint32_t d2;
  ms5525dso_qx_t qx;
  ms5525dso_coeff_t coeff;        //!< Coefficient table to use for calculations
  float pressure;                 //!< Compensated pressure in PSI
  float temp;                     //!< Compensated temperature in C
  pressor_sensor_state_t state;  //!< Internal state
} board_dev_ps_t;

/**
 * @brief
 *
 * @param ps
 * @param i2c_dev
 * @param qx
 */
void ps_init(board_dev_ps_t* ps, hal_i2c_dev_t i2c_dev, const ms5525dso_qx_t* qx);

/**
 * @brief
 *
 * @param ps
 * @return board_dev_status_t
 */
board_dev_status_t ps_update(board_dev_ps_t* ps);

/**
 * @brief
 *
 * @param ps
 * @return board_dev_status_t
 */
board_dev_status_t ps_get_status(board_dev_ps_t* ps);

/**
 * @brief
 *
 * @param ps
 * @param pressure
 * @return board_dev_status_t
 */
board_dev_status_t ps_get_pressure(board_dev_ps_t* ps, float* pressure);

/**
 * @brief
 *
 * @param ps
 * @param temperature
 * @return board_dev_status_t
 */
board_dev_status_t ps_get_temp(board_dev_ps_t* ps, float* temperature);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_BOARD_PS_H_
