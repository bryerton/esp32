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

#include <board_sw.h>

void sw_init(board_dev_sw_t* sw, hal_i2c_dev_t i2c_dev) {
  assert(sw);

  if (sw != NULL) {
    sw->i2c_dev = i2c_dev;
    sw->status = BOARD_DEV_NOT_READY;
  }
}

board_dev_status_t sw_set_channel(board_dev_sw_t* sw, uint8_t ch) {
  board_dev_status_t retval;
  hal_err_t res;

  assert(sw);

  retval = BOARD_DEV_NOT_READY;

  if (sw != NULL) {
    // Select the channel on the I2C switch that has the desired sensor on it
    res = tca9548a_write_channel(hal_i2c_get_config(HAL_I2C_DEV_SWITCH), ch);
    if (res == HAL_OK) {
      sw->status = BOARD_DEV_READY;
    }
    retval = sw->status;
  }

  return retval;
}

board_dev_status_t sw_get_channel(board_dev_sw_t* sw, uint8_t* ch) {
  board_dev_status_t retval;
  hal_err_t res;

  assert(sw);
  assert(ch);

  retval = BOARD_DEV_NOT_READY;

  if ((sw != NULL) && (ch != NULL)) {
    // Get the current channel from the switch
    res = tca9548a_read_channel(hal_i2c_get_config(HAL_I2C_DEV_SWITCH), ch);
    if (res == HAL_OK) {
      sw->status = BOARD_DEV_READY;
    }
    retval = sw->status;
  }

  return retval;
}
