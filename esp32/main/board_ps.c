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

#include <string.h>
#include <hal.h>
#include <board_ps.h>
#include <drv_i2c_ms5525dso.h>

static void update_state(board_dev_ps_t* ps, ps_state_t new_state);

void ps_init(board_dev_ps_t* ps, hal_i2c_dev_t i2c_dev, ms5525dso_osr_t osr,
             const ms5525dso_qx_t* qx) {
  assert(ps);
  assert(qx);

  if ((ps != NULL) && (qx != NULL)) {
    ps->status = BOARD_DEV_NOT_READY;
    ps->i2c_dev = i2c_dev;
    ps->temp = 0.0f;
    ps->pressure = 0.0f;
    ps->osr = osr;
    memcpy(&ps->qx, qx, sizeof(ms5525dso_qx_t));
    update_state(ps, PS_SENSOR_ST_RESET);
  }
}

board_dev_status_t ps_update(board_dev_ps_t* ps, ps_values_t* ps_values) {
  hal_err_t res;
  board_dev_status_t retval;

  assert(ps);
  assert(ps_values);

  retval = BOARD_DEV_NOT_READY;

  if ((ps != NULL) && (ps_values != NULL)) {
    switch (ps->state) {
      case PS_SENSOR_ST_RESET:
        ps->ts_state = hal_get_timestamp();
        ps->status = BOARD_DEV_NOT_READY;
        res = ms5525dso_soft_reset(hal_i2c_get_config(ps->i2c_dev));
        if (res == HAL_OK) {
          update_state(ps, PS_SENSOR_ST_CONFIG);
        }
        break;

      case PS_SENSOR_ST_CONFIG:
        if (hal_get_timestamp() >= (ps->ts_state + BOARD_PS_RESET_TIME)) {
          res = ms5525dso_read_all_coeff(hal_i2c_get_config(ps->i2c_dev),
                                         &ps->coeff);
          if (res == HAL_OK) {
            hal_log(HAL_LOG_INFO, "PS1", "Coefficient table:");
            hal_log(HAL_LOG_INFO, "PS1", "0 - 0x%.08X", ps->coeff.c[0]);
            hal_log(HAL_LOG_INFO, "PS1", "1 - 0x%.08X", ps->coeff.c[1]);
            hal_log(HAL_LOG_INFO, "PS1", "2 - 0x%.08X", ps->coeff.c[2]);
            hal_log(HAL_LOG_INFO, "PS1", "3 - 0x%.08X", ps->coeff.c[3]);
            hal_log(HAL_LOG_INFO, "PS1", "4 - 0x%.08X", ps->coeff.c[4]);
            hal_log(HAL_LOG_INFO, "PS1", "5 - 0x%.08X", ps->coeff.c[5]);
            hal_log(HAL_LOG_INFO, "PS1", "6 - 0x%.08X", ps->coeff.c[6]);
            hal_log(HAL_LOG_INFO, "PS1", "7 - 0x%.08X", ps->coeff.c[7]);

            res = ms5525dso_start_ch_convert(hal_i2c_get_config(ps->i2c_dev),
                                             MS5525DSO_CH_D1_PRESSURE, ps->osr);
          }
          if (res == HAL_OK) {
            update_state(ps, PS_SENSOR_ST_READ_CH1);
          } else {
            update_state(ps, PS_SENSOR_ST_RESET);
          }
        }
        break;

      case PS_SENSOR_ST_READ_CH1:
        // Has the previous conversion finished?
        if (hal_get_timestamp() >=
            (ps->ts_state + ms5525dso_get_conversion_time(ps->osr))) {
          res = ms5525dso_read_adc(hal_i2c_get_config(ps->i2c_dev), &ps->d1);
          if (res == HAL_OK) {
            res = ms5525dso_start_ch_convert(hal_i2c_get_config(ps->i2c_dev),
                                             MS5525DSO_CH_D2_TEMPERATURE,
                                             ps->osr);
          }

          if (res == HAL_OK) {
            // Store the time when we successfully started the pressure
            // conversion
            ps->ts_current_update = hal_get_timestamp();
            update_state(ps, PS_SENSOR_ST_READ_CH2);
          } else {
            update_state(ps, PS_SENSOR_ST_RESET);
          }
        }
        break;

      case PS_SENSOR_ST_READ_CH2:
        // Has the previous conversion finished?
        if (hal_get_timestamp() >=
            (ps->ts_state + ms5525dso_get_conversion_time(ps->osr))) {
          res = ms5525dso_read_adc(hal_i2c_get_config(ps->i2c_dev), &ps->d2);
          if (res == HAL_OK) {
            // Start the conversion again for channel 1
            res = ms5525dso_start_ch_convert(hal_i2c_get_config(ps->i2c_dev),
                                             MS5525DSO_CH_D1_PRESSURE,
                                             MS5525DSO_OSR256);
            // Calculate the new calibrated pressure and temp for the previously
            // read out p+t
            ms5525dso_calculate_pt(&ps->qx, &ps->coeff, ps->d1, ps->d2,
                                   &ps->pressure, &ps->temp);

            // Successfully updated pressure and temperature
            // so updated timestamp of when the update occurred
            ps->ts_last_update = ps->ts_current_update;
          }
          if (res == HAL_OK) {
            ps->status = BOARD_DEV_READY;
            update_state(ps, PS_SENSOR_ST_READ_CH1);
          } else {
            update_state(ps, PS_SENSOR_ST_RESET);
          }
        }
        break;

      default:
        ps->status = BOARD_DEV_NOT_READY;
        update_state(ps, PS_SENSOR_ST_RESET);
        break;
    }

    // Fill in values based on last readback pressure and temp
    ps_values->ts = ps->ts_last_update;
    ps_values->pressure = ps->pressure;
    ps_values->temp = ps->temp;

    retval = ps->status;
  }

  return retval;
}

ps_info_t* ps_get_info(board_dev_ps_t* ps, ps_info_t* info) {
  assert(ps);
  assert(info);

  if ((ps != NULL) && (info != NULL)) {
    memcpy(&info->coeff, &ps->coeff, sizeof(ps->coeff));
    memcpy(&info->qx, &ps->qx, sizeof(ps->qx));
    info->osr = ps->osr;
  }

  return info;
}

static void update_state(board_dev_ps_t* ps, ps_state_t new_state) {
  assert(ps);

  if (ps != NULL) {
    ps->state = new_state;
    ps->ts_state = hal_get_timestamp();
  }
}
