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
#include <stdlib.h>
#include <hal.h>
#include <drv_i2c_ads1115.h>

hal_err_t ads1115_read_conversion(hal_i2c_config_t* cfg, int16_t* value) {
  hal_err_t res;

  assert(cfg);
  assert(value);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (value != NULL)) {
    uint8_t buff[2];

    res = hal_i2c_read(cfg, buff, sizeof(buff));
    if (res == HAL_OK) {
      *value = (buff[0] << 8) | buff[1];
    }
  }

  return res;
}

hal_err_t ads1115_write_config(hal_i2c_config_t* cfg,
                               ads1115_config_t* config) {
  hal_err_t res;

  assert(cfg);
  assert(config);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (config != NULL)) {
    uint8_t buff[3];
    uint16_t config_word;

    config_word =
        ((config->one_shot & 0x01) << 15) | ((config->mux & 0x07) << 12) |
        ((config->pga & 0x07) << 9) | ((config->mode & 0x01) << 8) |
        ((config->data_rate & 0x07) << 5) | ((config->comp_mode & 0x01) << 4) |
        ((config->comp_pol & 0x01) << 3) | ((config->comp_lat & 0x01) << 2) |
        (config->comp_que & 0x03);

    buff[0] = ADS1115_REG_CONFIG;
    buff[1] = (config_word & 0xFF00) >> 8;
    buff[2] = (config_word & 0x00FF);

    res = hal_i2c_write(cfg, buff, sizeof(buff));
  }

  return res;
}

hal_err_t ads1115_read_config(hal_i2c_config_t* cfg, ads1115_config_t* config) {
  hal_err_t res;

  assert(cfg);
  assert(config);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (config != NULL)) {
    uint8_t buff[2];
    uint16_t config_word;

    buff[0] = ADS1115_REG_CONFIG;

    res = hal_i2c_write(cfg, buff, 1);
    if (res == HAL_OK) {
      res = hal_i2c_read(cfg, buff, sizeof(buff));
    }

    if (res == HAL_OK) {
      config_word = (buff[0] << 8) | buff[1];

      config->one_shot = (config_word >> 15) & 0x01;
      config->mux = (config_word >> 12) & 0x07;
      config->pga = (config_word >> 9) & 0x07;
      config->mode = (config_word >> 8) & 0x01;
      config->data_rate = (config_word >> 5) & 0x07;
      config->comp_mode = (config_word >> 4) & 0x01;
      config->comp_pol = (config_word >> 3) & 0x01;
      config->comp_lat = (config_word >> 2) & 0x01;
      config->comp_que = config_word & 0x03;
    }
  }

  return res;
}

hal_err_t ads1115_write_hi_thresh(hal_i2c_config_t* cfg, int16_t* value) {
  hal_err_t res;

  assert(cfg);
  assert(value);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (value != NULL)) {
    uint8_t buff[3];

    buff[0] = ADS1115_REG_HI_THRESH;
    buff[1] = (*value & 0xFF00) >> 8;
    buff[2] = (*value & 0x00FF);
    res = hal_i2c_write(cfg, buff, 1);
  }

  return res;
}

hal_err_t ads1115_read_hi_thresh(hal_i2c_config_t* cfg, int16_t* value) {
  hal_err_t res;

  assert(cfg);
  assert(value);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (value != NULL)) {
    uint8_t buff[2];

    buff[0] = ADS1115_REG_HI_THRESH;
    res = hal_i2c_write(cfg, buff, 1);
    if (res == HAL_OK) {
      res = hal_i2c_read(cfg, buff, sizeof(buff));
    }

    if (res == HAL_OK) {
      *value = (buff[0] << 8) | buff[1];
    }
  }

  return res;
}

hal_err_t ads1115_write_lo_thresh(hal_i2c_config_t* cfg, int16_t* value) {
  hal_err_t res;

  assert(cfg);
  assert(value);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (value != NULL)) {
    uint8_t buff[3];

    buff[0] = ADS1115_REG_LO_THRESH;
    buff[1] = (*value & 0xFF00) >> 8;
    buff[2] = (*value & 0x00FF);
    res = hal_i2c_write(cfg, buff, 1);
  }

  return res;
}

hal_err_t ads1115_read_lo_thresh(hal_i2c_config_t* cfg, int16_t* value) {
  hal_err_t res;

  assert(cfg);
  assert(value);

  res = HAL_ERR_FAIL;

  if ((cfg != NULL) && (value != NULL)) {
    uint8_t buff[2];

    buff[0] = ADS1115_REG_LO_THRESH;
    res = hal_i2c_write(cfg, buff, 1);
    if (res == HAL_OK) {
      res = hal_i2c_read(cfg, buff, sizeof(buff));
    }

    if (res == HAL_OK) {
      *value = (buff[0] << 8) | buff[1];
    }
  }

  return res;
}
