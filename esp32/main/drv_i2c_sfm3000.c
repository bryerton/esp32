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
#include <hal.h>
#include <drv_i2c_sfm3000.h>
#include <stdint.h>

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

static uint8_t crc8(const uint8_t *buff, uint8_t len);
static hal_err_t read_2byte(const hal_i2c_config_t *cfg, uint16_t *buffer);
static hal_err_t read_4byte(const hal_i2c_config_t *cfg, uint32_t *buffer);

hal_err_t sfm3000_soft_reset(const hal_i2c_config_t *cfg) {
  uint8_t cmd[2];

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  cmd[0] = SFM3000_REG_SOFT_RESET >> 8;
  cmd[1] = SFM3000_REG_SOFT_RESET & 0xFF;

  return hal_i2c_write(cfg, cmd, sizeof(cmd));
}

hal_err_t sfm3000_start_flow(const hal_i2c_config_t *cfg) {
  uint8_t cmd[2];

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  cmd[0] = SFM3000_REG_START_FLOW >> 8;
  cmd[1] = SFM3000_REG_START_FLOW & 0xFF;

  return hal_i2c_write(cfg, cmd, sizeof(cmd));
}

hal_err_t sfm3000_start_temp(const hal_i2c_config_t *cfg) {
  uint8_t cmd[2];

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  cmd[0] = SFM3000_REG_START_TEMP >> 8;
  cmd[1] = SFM3000_REG_START_TEMP & 0xFF;

  return hal_i2c_write(cfg, cmd, sizeof(cmd));
}

hal_err_t sfm3000_read_flow(const hal_i2c_config_t *cfg, uint16_t *flow_raw) {
  hal_err_t res;
  uint16_t value;

  assert(cfg && flow_raw);
  if (!cfg || !flow_raw) {
    return HAL_ERR_FAIL;
  }

  *flow_raw = 0.0f;

  res = read_2byte(cfg, &value);
  if (res != HAL_OK) {
    return res;
  }

  *flow_raw = value;

  return res;
}

hal_err_t sfm3000_read_temp(const hal_i2c_config_t *cfg, float *temp) {
  hal_err_t res;
  uint16_t value;

  assert(cfg && temp);
  if (!cfg || !temp) {
    return HAL_ERR_FAIL;
  }

  *temp = 0.0f;

  res = read_2byte(cfg, &value);
  if (res != HAL_OK) {
    return res;
  }

  // @TODO: Test result of function and manually determine formula. Datasheet
  // does not specify the temperature format
  *temp = value;

  return res;
}

hal_err_t sfm3000_read_scale_factor(const hal_i2c_config_t *cfg,
                                    float *scale_factor) {
  uint8_t cmd[2];
  hal_err_t res;
  uint16_t value;

  assert(cfg && scale_factor);
  if (!cfg || !scale_factor) {
    return HAL_ERR_FAIL;
  }

  *scale_factor = 0;

  cmd[0] = SFM3000_REG_SCALE_FACTOR >> 8;
  cmd[1] = SFM3000_REG_SCALE_FACTOR & 0xFF;

  res = hal_i2c_write(cfg, cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  res = read_2byte(cfg, &value);
  if (res != HAL_OK) {
    return res;
  }

  *scale_factor = value;

  return res;
}

hal_err_t sfm3000_read_offset(const hal_i2c_config_t *cfg, float *offset) {
  uint8_t cmd[2];
  hal_err_t res;
  uint16_t value;

  assert(cfg && offset);
  if (!cfg || !offset) {
    return HAL_ERR_FAIL;
  }

  *offset = 0;

  cmd[0] = SFM3000_REG_OFFSET >> 8;
  cmd[1] = SFM3000_REG_OFFSET & 0xFF;

  res = hal_i2c_write(cfg, cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  res = read_2byte(cfg, &value);
  if (res != HAL_OK) {
    return res;
  }

  *offset = value;

  return res;
}

hal_err_t sfm3000_read_serial(const hal_i2c_config_t *cfg, uint32_t *serial) {
  uint8_t cmd[2];
  hal_err_t res;

  assert(cfg && serial);
  if (!cfg || !serial) {
    return HAL_ERR_FAIL;
  }

  *serial = 0;

  cmd[0] = SFM3000_REG_SERIAL_HI >> 8;
  cmd[1] = SFM3000_REG_SERIAL_HI & 0xFF;

  res = hal_i2c_write(cfg, cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  return read_4byte(cfg, serial);
}

hal_err_t sfm3000_read_product(const hal_i2c_config_t *cfg, uint32_t *product) {
  uint8_t cmd[2];
  hal_err_t res;

  assert(cfg && product);
  if (!cfg || !product) {
    return HAL_ERR_FAIL;
  }

  *product = 0;

  cmd[0] = SFM3000_REG_PRODUCT_HI >> 8;
  cmd[1] = SFM3000_REG_PRODUCT_HI & 0xFF;

  res = hal_i2c_write(cfg, cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  return read_4byte(cfg, product);
}

void sfm3000_convert_to_slm(uint16_t flow_raw,
                            const sfm3000_settings_t *settings, float *flow) {
  assert(settings && flow);
  if (!settings || !flow) {
    return;
  }

  // Protect against divide by zero
  // As the given scale factors are in the ~140 range, this should never occur
  // And can be considered an error
  if (settings->scale_factor > 1.0f) {
    *flow = (flow_raw - settings->offset) / settings->scale_factor;
  } else {
    *flow = 0.0f;
  }
}

static hal_err_t read_2byte(const hal_i2c_config_t *cfg, uint16_t *buffer) {
  hal_err_t res;
  uint8_t buff[3];  // First two bytes are data, third is crc

  assert(cfg && buffer);
  if (!cfg || !buffer) {
    return HAL_ERR_FAIL;
  }

  *buffer = 0;

  res = hal_i2c_read(cfg, buff, sizeof(buff));
  if (res != HAL_OK) {
    return res;
  }

  // Check CRC (third byte) against calculated CRC value
  if (crc8(buff, 2) != buff[2]) {
    return HAL_ERR_FAIL;
  }

  // MSB first
  *buffer = (buff[0] << 8) | buff[1];

  return res;
}

static hal_err_t read_4byte(const hal_i2c_config_t *cfg, uint32_t *buffer) {
  hal_err_t res;
  uint8_t buff[6];

  assert(cfg && buffer);
  if (!cfg || !buffer) {
    return HAL_ERR_FAIL;
  }

  *buffer = 0;

  res = hal_i2c_read(cfg, buff, sizeof(buff));
  if (res != HAL_OK) {
    return res;
  }

  // Check CRCs (third byte and sixth byte) against calculated crc values
  if ((crc8(&buff[0], 2) != buff[2]) || (crc8(&buff[3], 2) != buff[5])) {
    return HAL_ERR_FAIL;
  }

  // MSB first, make sure to not stuff the CRC bytes in!
  *buffer = (buff[0] << 24) | (buff[1] << 16) | (buff[3] << 8) | buff[4];

  return res;
}

static uint8_t crc8(const uint8_t *buff, uint8_t len) {
  uint8_t n;
  uint8_t crc;

  assert(buff);
  if (!buff) {
    return 0;
  }

  crc = 0x00;
  for (n = 0; n < len; n++) {
    crc = crc8_table[crc ^ buff[n]];
  }

  return crc;
}
