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
#include "drv_i2c_ms5525dso.h"

static uint32_t expected_response_len;
static uint32_t last_requested_cmd;
static uint32_t skip_count;
static uint32_t fail_count;
static uint32_t fail_crc;

void setUp(void) {}

void tearDown(void) {}

void test_drv_i2c_ms5525dso_soft_reset(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;

  res = ms5525dso_soft_reset(0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = res = ms5525dso_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_ms5525dso_read_prom(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  uint16_t prom_value;
  uint32_t n;

  res = ms5525dso_read_prom(0, 0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_prom(0, 0, &prom_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_prom(&cfg, 0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = ms5525dso_read_prom(&cfg, 0, &prom_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = ms5525dso_read_prom(&cfg, 0, &prom_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  // Internally this code enforces only accessing 0-7, so not possible
  // To test 'overrange', as it gets AND'd down to 0-7
  for (n = 0; n < 8; n++) {
    res = ms5525dso_read_prom(&cfg, n, &prom_value);
    TEST_ASSERT_EQUAL(HAL_OK, res);
  }
}

void test_drv_i2c_ms5525dso_read_all_coeff(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  ms5525dso_coeff_t coeff;

  res = ms5525dso_read_all_coeff(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_all_coeff(0, &coeff);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_all_coeff(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_ms5525dso_start_ch_convert(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  ms5525dso_coeff_t coeff;
  ms5525dso_ch_t ch;
  ms5525dso_osr_t osr;

  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = ms5525dso_start_ch_convert(0, 0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_start_ch_convert(&cfg, 3, MS5525DSO_OSR256);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_start_ch_convert(&cfg, 0, MS5525DSO_OSR4096 + 1);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = ms5525dso_start_ch_convert(&cfg, 0, MS5525DSO_OSR256);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  for (ch = 0; ch <= 1; ch++) {
    for (osr = MS5525DSO_OSR256; osr <= MS5525DSO_OSR4096; osr++) {
      res = ms5525dso_start_ch_convert(&cfg, ch, osr);
      TEST_ASSERT_EQUAL(HAL_OK, res);
    }
  }
}

void test_drv_i2c_ms5525dso_read_adc(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  uint32_t adc_value;

  res = ms5525dso_read_adc(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_adc(0, &adc_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_adc(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = ms5525dso_read_adc(&cfg, &adc_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = ms5525dso_read_adc(&cfg, &adc_value);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = ms5525dso_read_adc(&cfg, &adc_value);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_ms5525dso_calculate_coeff_crc(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  ms5525dso_coeff_t coeff;
  uint8_t crc;

  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  crc = ms5525dso_calculate_coeff_crc(0);
  TEST_ASSERT_EQUAL(0, crc);

  crc = ms5525dso_calculate_coeff_crc(&coeff);
  TEST_ASSERT_EQUAL(coeff.c[7] & 0xF, crc);
}

void test_drv_i2c_ms5525dso_calculate_pt(void) {
  hal_err_t res;
  hal_i2c_config_t cfg;
  ms5525dso_coeff_t coeff;
  uint32_t d1;
  uint32_t d2;
  float p;
  float t;
  ms5525dso_qx_t qx = MS5525DSO_QX_FOR_PP001DS();

  d1 = 4650976;
  d2 = 4946912;

  res = ms5525dso_read_all_coeff(&cfg, &coeff);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  ms5525dso_calculate_pt(0, 0, d1, d2, 0, 0);
  ms5525dso_calculate_pt(0, 0, d1, d2, 0, &t);
  ms5525dso_calculate_pt(0, 0, d1, d2, &t, 0);
  ms5525dso_calculate_pt(0, 0, d1, d2, &p, &t);

  ms5525dso_calculate_pt(0, &coeff, d1, d2, 0, 0);
  ms5525dso_calculate_pt(0, &coeff, d1, d2, 0, &t);
  ms5525dso_calculate_pt(0, &coeff, d1, d2, &t, 0);
  ms5525dso_calculate_pt(0, &coeff, d1, d2, &p, &t);

  ms5525dso_calculate_pt(&qx, 0, d1, d2, 0, 0);
  ms5525dso_calculate_pt(&qx, 0, d1, d2, 0, &t);
  ms5525dso_calculate_pt(&qx, 0, d1, d2, &t, 0);
  ms5525dso_calculate_pt(&qx, 0, d1, d2, &p, &t);

  ms5525dso_calculate_pt(&qx, &coeff, d1, d2, 0, 0);
  ms5525dso_calculate_pt(&qx, &coeff, d1, d2, 0, &t);
  ms5525dso_calculate_pt(&qx, &coeff, d1, d2, &t, 0);

  ms5525dso_calculate_pt(&qx, &coeff, d1, d2, &p, &t);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, -706.911f, p);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 163489.6f, t);

  ms5525dso_calculate_pt(&qx, &coeff, d1, d2, &p, &t);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, -706.911f, p);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 163489.6f, t);
}

hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len) {
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  // The buffer has to be the right length
  if (len != 1) {
    return HAL_ERR_FAIL;
  }

  if (skip_count) {
    skip_count--;
  } else if (fail_count) {
    fail_count--;
    return HAL_ERR_FAIL;
  }

  switch (buffer[0]) {
    case 0x1E:  // soft reset
      expected_response_len = 0;
      break;
    case 0x00:  // adc readback
      expected_response_len = 3;
      break;
    case 0xA0:  // prom address 0
      expected_response_len = 2;
      break;
    case 0xA2:  // prom address 1
      expected_response_len = 2;
      break;
    case 0xA4:  // prom address 2
      expected_response_len = 2;
      break;
    case 0xA6:  // prom address 3
      expected_response_len = 2;
      break;
    case 0xA8:  // prom address 4
      expected_response_len = 2;
      break;
    case 0xAA:  // prom address 5
      expected_response_len = 2;
      break;
    case 0xAC:  // prom address 6
      expected_response_len = 2;
      break;
    case 0xAE:  // prom address 7
      expected_response_len = 2;
      break;
    case 0x40:  // CH1 - 256
      expected_response_len = 0;
      break;
    case 0x42:  // CH1 - 512
      expected_response_len = 0;
      break;
    case 0x44:  // CH1 - 1024
      expected_response_len = 0;
      break;
    case 0x46:  // CH1 - 2048
      expected_response_len = 0;
      break;
    case 0x48:  // CH1 - 4096
      expected_response_len = 0;
      break;
    case 0x50:  // CH2 - 256
      expected_response_len = 0;
      break;
    case 0x52:  // CH2 - 512
      expected_response_len = 0;
      break;
    case 0x54:  // CH2 - 1024
      expected_response_len = 0;
      break;
    case 0x56:  // CH2 - 2048
      expected_response_len = 0;
      break;
    case 0x58:  // CH2 - 4096
      expected_response_len = 0;
      break;
    default:
      expected_response_len = 0;
      return HAL_ERR_FAIL;
  }

  last_requested_cmd = buffer[0];

  return HAL_OK;
}

hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len) {
  static unsigned int seed;
  // Response is expected + CRC
  // It's 'allowed' to requested just the flow data for example, but that's
  // crazy So lets enforce the CRC check length
  if (len != expected_response_len) {
    return HAL_ERR_FAIL;
  }

  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  if (!buffer) {
    return HAL_ERR_FAIL;
  }

  if (skip_count) {
    skip_count--;
  } else if (fail_count) {
    fail_count--;
    return HAL_ERR_FAIL;
  }

  switch (last_requested_cmd) {
    case 0x1E:  // soft reset
      return HAL_ERR_FAIL;
    case 0x00:  // adc readback
      buffer[0] = rand_r(&seed);
      buffer[1] = rand_r(&seed);
      buffer[2] = rand_r(&seed);
      break;
    case 0xA0:  // prom address 0
      buffer[0] = 0x00;
      buffer[1] = 0x01;
      break;
    case 0xA2:  // prom address 1
      buffer[0] = 0x31;
      buffer[1] = 0xDA;
      break;
    case 0xA4:  // prom address 2
      buffer[0] = 0x1B;
      buffer[1] = 0x42;
      break;
    case 0xA6:  // prom address 3
      buffer[0] = 0x0C;
      buffer[1] = 0x66;
      break;
    case 0xA8:  // prom address 4
      buffer[0] = 0x06;
      buffer[1] = 0xB9;
      break;
    case 0xAA:  // prom address 5
      buffer[0] = 0x95;
      buffer[1] = 0x0C;
      break;
    case 0xAC:  // prom address 6
      buffer[0] = 0x1F;
      buffer[1] = 0x37;
      break;
    case 0xAE:  // prom address 7
      if (fail_crc) {
        fail_crc--;
        buffer[0] = 0x00;
        buffer[1] = ~0x09;
      } else {
        buffer[0] = 0x00;
        buffer[1] = 0x09;
      }
      break;
    default:
      return HAL_ERR_FAIL;
  }

  return HAL_OK;
}
