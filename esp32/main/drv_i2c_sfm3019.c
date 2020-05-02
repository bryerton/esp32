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
#include <stdint.h>
#include <stdlib.h>
#include <hal.h>
#include <drv_i2c_sfm3019.h>

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

static uint8_t crc8(const uint8_t* buff, uint8_t len);
static hal_err_t write_cmd(const hal_i2c_config_t* cfg, uint16_t cmd);
static hal_err_t write_cmd_with_arg(const hal_i2c_config_t* cfg, uint16_t cmd,
                                    uint16_t arg);
static hal_err_t read_words(const hal_i2c_config_t* cfg, uint16_t* buffer,
                            uint32_t num_words);

hal_err_t sfm3019_start_cont_meas(const hal_i2c_config_t* cfg,
                                  sfm3019_gas_t gas, uint16_t fraction) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    switch (gas) {
      case SFM3019_GAS_O2:
        res = write_cmd(cfg, SFM3019_REG_GAS_O2);
        break;
      case SFM3019_GAS_AIR:
        res = write_cmd(cfg, SFM3019_REG_GAS_AIR);
        break;
      case SFM3019_GAS_MIX:
        res = write_cmd_with_arg(cfg, SFM3019_REG_GAS_MIX, fraction);
        break;
      default:
        break;
    }
  }

  return res;
}

hal_err_t sfm3019_read_meas(const hal_i2c_config_t* cfg, int16_t* flow,
                            int16_t* temp, int16_t* status) {
  hal_err_t res;
  uint16_t buffer[3];

  assert(cfg);
  assert(flow);
  assert(temp);
  assert(status);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (flow != NULL) && (temp != NULL) && (status != NULL)) {
    res = read_words(cfg, buffer, 3);
    if (res == HAL_OK) {
      *flow = (int16_t)buffer[0];
      *temp = (int16_t)buffer[1];
      *status = (int16_t)buffer[2];
    }
  }

  return res;
}

hal_err_t sfm3019_update_concentration(const hal_i2c_config_t* cfg,
                                       uint16_t fraction) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    res = write_cmd_with_arg(cfg, SFM3019_REG_UPDATE_CONC, fraction);
    if (res == HAL_OK) {
      res = write_cmd(cfg, SFM3019_REG_RESET_PTR);
    }
  }

  return res;
}

hal_err_t sfm3019_stop_cont_meas(const hal_i2c_config_t* cfg) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    res = write_cmd(cfg, SFM3019_REG_STOP_MEAS);
  }

  return res;
}

hal_err_t sfm3019_configure_avg(const hal_i2c_config_t* cfg, uint16_t average) {
  hal_err_t res;

  assert(cfg);
  assert(average <= 128);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (average <= 128)) {
    res = write_cmd_with_arg(cfg, SFM3019_REG_CONF_AVG, average);
  }

  return res;
}

hal_err_t sfm3019_read_settings(const hal_i2c_config_t* cfg, sfm3019_gas_t gas,
                                int16_t* scale_factor, int16_t* offset,
                                uint16_t* flow_unit) {
  hal_err_t res;
  uint16_t buffer[3];

  assert(cfg);
  assert(scale_factor);
  assert(offset);
  assert(flow_unit);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (scale_factor != NULL) && (offset != NULL) &&
      (flow_unit != NULL)) {
    switch (gas) {
      case SFM3019_GAS_O2:
        res = write_cmd_with_arg(cfg, SFM3019_REG_READ_SETTINGS,
                                 SFM3019_REG_GAS_O2);
        break;
      case SFM3019_GAS_AIR:
        res = write_cmd_with_arg(cfg, SFM3019_REG_READ_SETTINGS,
                                 SFM3019_REG_GAS_AIR);
        break;
      case SFM3019_GAS_MIX:
        res = write_cmd_with_arg(cfg, SFM3019_REG_READ_SETTINGS,
                                 SFM3019_REG_GAS_MIX);
        break;
      default:
        break;
    }

    if (res == HAL_OK) {
      res = read_words(cfg, buffer, 3);
      if (res == HAL_OK) {
        *scale_factor = (int16_t)buffer[0];
        *offset = (int16_t)buffer[1];
        *flow_unit = buffer[2];
      }
    }
  }

  return res;
}

hal_err_t sfm3019_read_product_ident(const hal_i2c_config_t* cfg,
                                     uint32_t* product, uint64_t* serial) {
  hal_err_t res;
  uint16_t buffer[6];

  assert(cfg);
  assert(product);
  assert(serial);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (product != NULL) && (serial != NULL)) {
    res = write_cmd(cfg, SFM3019_REG_READ_PRODUCT);

    if (res == HAL_OK) {
      res = read_words(cfg, buffer, 6);
      if (res == HAL_OK) {
        *product = ((uint32_t)buffer[0] << 16) | (uint32_t)buffer[1];
        *serial = ((uint64_t)buffer[2] << 48) | ((uint64_t)buffer[3] << 32) |
                  ((uint64_t)buffer[4] << 16) | (uint64_t)buffer[5];
      }
    }
  }

  return res;
}

static hal_err_t write_cmd(const hal_i2c_config_t* cfg, uint16_t cmd) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    uint8_t buff[2];

    buff[0] = (cmd & 0xFF00u) >> 8;
    buff[1] = (cmd & 0x00FFu);
    res = hal_i2c_write(cfg, buff, sizeof(buff));
  }

  return res;
}

static hal_err_t write_cmd_with_arg(const hal_i2c_config_t* cfg, uint16_t cmd,
                                    uint16_t arg) {
  hal_err_t res;

  assert(cfg);

  res = HAL_ERR_FAIL;

  if (cfg != NULL) {
    uint8_t buff[5];

    buff[0] = (cmd & 0xFF00u) >> 8;
    buff[1] = (cmd & 0x00FFu);
    buff[2] = (arg & 0xFF00u) >> 8;
    buff[3] = (arg & 0x00FFu);
    buff[4] = crc8(&buff[2], 2);

    res = hal_i2c_write(cfg, buff, sizeof(buff));
  }

  return res;
}

static hal_err_t read_words(const hal_i2c_config_t* cfg, uint16_t* buffer,
                            uint32_t num_words) {
  hal_err_t res;
  // maximum possible read is 6 words = 12 bytes + 6 crc bytes
  uint8_t read_buffer[18];
  uint8_t num_bytes_to_read;

  assert(cfg);
  assert(buffer);
  assert(num_words > 6);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (buffer != NULL) && (num_words <= 6) &&
      (num_words % 2 == 0)) {
    num_bytes_to_read = num_words * 3;
    uint8_t failed_crc;

    res = hal_i2c_read(cfg, read_buffer, num_bytes_to_read);
    if (res == HAL_OK) {
      uint8_t word;
      for (word = 0, failed_crc = 0; (word < num_words) && (res == HAL_OK);
           word++) {
        if (crc(&read_buffer[(word * 3)], 2) != read_buffer[(word * 3) + 2]) {
          res = HAL_ERR_FAIL;
        } else {
          buffer[word] =
              (read_buffer[(word * 3)] << 8) | read_buffer[(word * 3) + 1];
        }
      }
    }
  }

  return res;
}

static uint8_t crc8(const uint8_t* buff, uint8_t len) {
  uint8_t crc;

  assert(buff);

  crc = 0xFF;

  if (buff != NULL) {
    for (uint8_t n = 0; n < len; n++) {
      crc = crc8_table[crc ^ buff[n]];
    }
  }

  return crc;
}
