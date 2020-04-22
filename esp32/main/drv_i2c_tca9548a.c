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

#include <assert.h>
#include <drv_i2c_tca9548a.h>
#include <hal.h>
#include <stdint.h>

hal_err_t tca9548a_write_channel(const hal_i2c_config_t* cfg,
                                 uint8_t ch_bitmask) {
  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  return hal_i2c_write(cfg, &ch_bitmask, 1);
}

hal_err_t tca9548a_read_channel(const hal_i2c_config_t* cfg,
                                uint8_t* ch_bitmask) {
  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  assert(ch_bitmask);
  if (!ch_bitmask) {
    return HAL_ERR_FAIL;
  }

  // Set the channel return to TCA9548A_INVALID_CH by default
  // this is not a legit channel mask so can be used as error detect
  *ch_bitmask = TCA9548A_INVALID_CH;

  return hal_i2c_read(cfg, ch_bitmask, 1);
}
