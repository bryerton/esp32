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

#include <stdio.h>
#include <stdlib.h>
#include <unity.h>
#include "drv_i2c_tca9548a.h"

static uint8_t current_ch_mask;

void setUp(void) { current_ch_mask = 0; }

void tearDown(void) { current_ch_mask = 0; }

void test_drv_i2c_tca9548a_write_fail(void) {
  hal_err_t res;

  res = tca9548a_write_channel(0, 1);

  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);
}

void test_drv_i2c_tca9548a_read_fail(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint8_t ch_mask;

  res = tca9548a_read_channel(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = tca9548a_read_channel(0, &ch_mask);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = tca9548a_read_channel(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);
}

void test_drv_i2c_tca9548a_write_channel(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint8_t ch;

  res = tca9548a_write_channel(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_tca9548a_read_channel(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint8_t ch;
  uint32_t n;

  for (n = 0; n <= 255; n++) {
    res = tca9548a_write_channel(&cfg, n);
    TEST_ASSERT_EQUAL(HAL_OK, res);

    res = tca9548a_read_channel(&cfg, &ch);
    TEST_ASSERT_EQUAL(HAL_OK, res);
    TEST_ASSERT_EQUAL(n, ch);
  }
}

hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len) {
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  if (len != 1) {
    return HAL_ERR_FAIL;
  }

  current_ch_mask = buffer[0];

  return HAL_OK;
}

hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len) {
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  if (!buffer) {
    return HAL_ERR_FAIL;
  }

  if (len != 1) {
    return HAL_ERR_FAIL;
  }

  buffer[0] = current_ch_mask;

  return HAL_OK;
}
