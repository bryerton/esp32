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

#include <board.h>
#include <board_sw.h>

void sw_init(board_dev_sw_t* sw, hal_i2c_dev_t i2c_dev) {
  assert(sw);
  if (!sw) {
    return;
  }

  sw->i2c_dev = i2c_dev;
  sw->status = BOARD_DEV_NOT_READY;
}

board_dev_status_t sw_set_channel(board_dev_sw_t* sw, uint8_t ch) {
  hal_err_t res;

  assert(sw);
  if (!sw) {
    return BOARD_DEV_NOT_READY;
  }

  // Select the channel on the I2C switch that has the desired sensor on it
  res = tca9548a_write_channel(hal_i2c_get_config(HAL_I2C_DEV_SWITCH), ch);
  if (res == HAL_OK) {
    sw->status = BOARD_DEV_READY;
  } else {
    sw->status = BOARD_DEV_NOT_READY;
  }

  return sw->status;
}

board_dev_status_t sw_get_channel(board_dev_sw_t* sw, uint8_t* ch) {
  hal_err_t res;

  assert(sw && ch);
  if (!ch || !sw) {
    return BOARD_DEV_NOT_READY;
  }

  // Get the current channel from the switch
  res = tca9548a_read_channel(hal_i2c_get_config(HAL_I2C_DEV_SWITCH), ch);
  if (res == HAL_OK) {
    sw->status = BOARD_DEV_READY;
  } else {
    sw->status = BOARD_DEV_NOT_READY;
  }

  return sw->status;
}

board_dev_status_t sw_get_status(const board_dev_sw_t* sw) {
  assert(sw);
  if (!sw) {
    return BOARD_DEV_NOT_READY;
  }

  return sw->status;
}
