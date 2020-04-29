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

#include <assert.h>
#include <drv_i2c_tca9548a.h>
#include <hal.h>
#include <stdint.h>
#include <stdlib.h>

hal_err_t tca9548a_write_channel(const hal_i2c_config_t* cfg,
                                 uint8_t ch_bitmask) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    res = hal_i2c_write(cfg, &ch_bitmask, sizeof(uint8_t));
  }

  return res;
}

hal_err_t tca9548a_read_channel(const hal_i2c_config_t* cfg,
                                uint8_t* ch_bitmask) {
  hal_err_t res;

  assert(cfg);
  assert(ch_bitmask);

  res = HAL_ERR_FAIL;
  if ((cfg != NULL) && (ch_bitmask != NULL)) {
    res = hal_i2c_read(cfg, ch_bitmask, sizeof(uint8_t));
  }

  return res;
}
