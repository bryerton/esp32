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
#include "drv_i2c_sfm3000.h"

static uint8_t crc8(const uint8_t* buff, uint8_t len);

static uint32_t expected_response_len;
static uint32_t last_requested_cmd;
static uint8_t skip_count;
static uint8_t fail_count;
static uint8_t skip_crc;
static uint8_t fail_crc;

void setUp(void) {}

void tearDown(void) {}

void test_drv_i2c_sfm3000_soft_reset(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;

  res = sfm3000_soft_reset(0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_sfm3000_start_flow(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;

  res = sfm3000_start_flow(0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_start_flow(&cfg);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_start_flow(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_sfm3000_read_flow(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint16_t flow_raw;

  // Reset flow sensor
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = sfm3000_read_flow(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_flow(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  // This fails because start_flow() hasn't been called yet
  res = sfm3000_read_flow(&cfg, &flow_raw);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_start_flow(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  fail_count = 1;
  res = sfm3000_read_flow(&cfg, &flow_raw);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = sfm3000_read_flow(&cfg, &flow_raw);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_flow(&cfg, &flow_raw);
  TEST_ASSERT_EQUAL(HAL_OK, res);
}

void test_drv_i2c_sfm3000_read_scale_factor(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  float scale_factor;

  // Reset flow sensor
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = sfm3000_read_scale_factor(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_scale_factor(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_read_scale_factor(&cfg, &scale_factor);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = sfm3000_read_scale_factor(&cfg, &scale_factor);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = sfm3000_read_scale_factor(&cfg, &scale_factor);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_scale_factor(&cfg, &scale_factor);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 140.0f, scale_factor);
}

void test_drv_i2c_sfm3000_read_offset(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  float offset;

  // Reset flow sensor
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = sfm3000_read_offset(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_offset(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_read_offset(&cfg, &offset);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = sfm3000_read_offset(&cfg, &offset);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = sfm3000_read_offset(&cfg, &offset);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_offset(&cfg, &offset);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 32000.0f, offset);
}

void test_drv_i2c_sfm3000_read_serial(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint32_t serial;

  // Reset flow sensor
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = sfm3000_read_serial(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_serial(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_read_serial(&cfg, &serial);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = sfm3000_read_serial(&cfg, &serial);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = sfm3000_read_serial(&cfg, &serial);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_crc = 1;
  fail_crc = 1;
  res = sfm3000_read_serial(&cfg, &serial);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_serial(&cfg, &serial);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  TEST_ASSERT_EQUAL(0x01020304, serial);
}

void test_drv_i2c_sfm3000_read_product(void) {
  hal_i2c_config_t cfg;
  hal_err_t res;
  uint32_t product;

  // Reset flow sensor
  res = sfm3000_soft_reset(&cfg);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  res = sfm3000_read_product(0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_product(&cfg, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_count = 1;
  res = sfm3000_read_product(&cfg, &product);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_count = 1;
  fail_count = 1;
  res = sfm3000_read_product(&cfg, &product);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  fail_crc = 1;
  res = sfm3000_read_product(&cfg, &product);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  skip_crc = 1;
  fail_crc = 1;
  res = sfm3000_read_product(&cfg, &product);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_read_product(&cfg, &product);
  TEST_ASSERT_EQUAL(HAL_OK, res);

  TEST_ASSERT_EQUAL(0x118A4602, product);
}

void test_drv_i2c_sfm3000_convert_to_slm(void) {
  hal_err_t res;
  sfm3000_settings_t settings;
  float slm;

  res = sfm3000_convert_to_slm(0, 0, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_convert_to_slm(0, 0, &slm);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  res = sfm3000_convert_to_slm(0, &settings, 0);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  settings.offset = 0.0f;
  settings.scale_factor = 0.0f;

  res = sfm3000_convert_to_slm(0, &settings, &slm);
  TEST_ASSERT_EQUAL(HAL_ERR_FAIL, res);

  settings.offset = 32000.0f;
  settings.scale_factor = 140.0f;

  res = sfm3000_convert_to_slm(0, &settings, &slm);
  TEST_ASSERT_EQUAL(HAL_OK, res);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, -228.57f, slm);

  res = sfm3000_convert_to_slm(65535, &settings, &slm);
  TEST_ASSERT_EQUAL(HAL_OK, res);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 239.53, slm);

  res = sfm3000_convert_to_slm(32768, &settings, &slm);
  TEST_ASSERT_EQUAL(HAL_OK, res);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.48f, slm);
}

static const uint8_t crc8_table[] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97, 0xB9, 0x88, 0xDB, 0xEA,
    0x7D, 0x4C, 0x1F, 0x2E, 0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D, 0x86, 0xB7, 0xE4, 0xD5,
    0x42, 0x73, 0x20, 0x11, 0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7C, 0x4D, 0x1E, 0x2F,
    0xB8, 0x89, 0xDA, 0xEB, 0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13, 0x7E, 0x4F, 0x1C, 0x2D,
    0xBA, 0x8B, 0xD8, 0xE9, 0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C, 0x02, 0x33, 0x60, 0x51,
    0xC6, 0xF7, 0xA4, 0x95, 0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6, 0x7A, 0x4B, 0x18, 0x29,
    0xBE, 0x8F, 0xDC, 0xED, 0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE, 0x80, 0xB1, 0xE2, 0xD3,
    0x44, 0x75, 0x26, 0x17, 0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2, 0xBF, 0x8E, 0xDD, 0xEC,
    0x7B, 0x4A, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0, 0xFE, 0xCF, 0x9C, 0xAD,
    0x3A, 0x0B, 0x58, 0x69, 0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A, 0xC1, 0xF0, 0xA3, 0x92,
    0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15, 0x3B, 0x0A, 0x59, 0x68,
    0xFF, 0xCE, 0x9D, 0xAC};

hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len) {
  uint16_t requested_cmd;

  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  // The buffer has to be the right length
  if (len != 2) {
    return HAL_ERR_FAIL;
  }

  if (skip_count) {
    skip_count--;
  } else if (fail_count) {
    fail_count--;
    return HAL_ERR_FAIL;
  }

  requested_cmd = (buffer[0] << 8) | buffer[1];

  switch (requested_cmd) {
    case 0x1000:  // start flow
      expected_response_len = 2;
      break;
    case 0x30DE:  // scale factor
      expected_response_len = 2;
      break;
    case 0x30DF:  // offset
      expected_response_len = 2;
      break;
    case 0x31E3:  // product id
      expected_response_len = 4;
      break;
    case 0x31AE:  // serial number
      expected_response_len = 4;
      break;
    case 0x2000:
      expected_response_len = 0;
      break;
    default:
      return HAL_ERR_FAIL;
  }

  last_requested_cmd = requested_cmd;

  return HAL_OK;
}

hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len) {
  static unsigned int seed;

  // Response is expected + CRC
  // It's 'allowed' to requested just the flow data for example, but that's
  // crazy So lets enforce the CRC check length
  if (len != expected_response_len + (expected_response_len / 2)) {
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
    case 0x1000:  // start flow
      buffer[0] = rand_r(&seed);
      buffer[1] = rand_r(&seed);
      buffer[2] = crc8(buffer, 2);
      break;
    case 0x30DE:  // scale factor
      buffer[0] = 140 >> 8;
      buffer[1] = 140 & 0x00FF;
      buffer[2] = crc8(buffer, 2);
      break;
    case 0x30DF:  // offset
      buffer[0] = 32000 >> 8;
      buffer[1] = 32000 & 0x00FF;
      buffer[2] = crc8(buffer, 2);
      break;
    case 0x31E3:  // product id
      buffer[0] = 0x11;
      buffer[1] = 0x8A;
      buffer[2] = crc8(buffer, 2);
      buffer[3] = 0x46;
      buffer[4] = 0x02;
      buffer[5] = crc8(&buffer[3], 2);
      break;
    case 0x31AE:  // serial number
      buffer[0] = 0x01;
      buffer[1] = 0x02;
      buffer[2] = crc8(buffer, 2);
      buffer[3] = 0x03;
      buffer[4] = 0x04;
      buffer[5] = crc8(&buffer[3], 2);
      break;
    case 0x2000:  // can't just read after the reset!
      return HAL_ERR_FAIL;
    default:
      return HAL_ERR_FAIL;
  }

  return HAL_OK;
}

static uint8_t crc8(const uint8_t* buff, uint8_t len) {
  uint8_t n;
  uint8_t crc;

  if (!buff) {
    return 0;
  }

  crc = 0x00;
  for (n = 0; n < len; n++) {
    crc = crc8_table[crc ^ buff[n]];
  }

  if (skip_crc) {
    skip_crc--;
  } else if (fail_crc) {
    fail_crc--;
    crc = ~crc;  // ensure we don't have a match to the right crc
  }

  return crc;
}
