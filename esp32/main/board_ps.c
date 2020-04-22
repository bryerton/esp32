#include <string.h>
#include <hal.h>
#include <board.h>
#include <board_ps.h>
#include <drv_i2c_ms5525dso.h>

void ps_init(board_dev_ps_t* ps, hal_i2c_dev_t i2c_dev,
             const ms5525dso_qx_t* qx) {
  assert(ps && qx);
  if (!ps || !qx) {
    return;
  }

  ps->status = BOARD_DEV_NOT_READY;
  ps->ts_state = 0;
  ps->i2c_dev = i2c_dev;
  ps->state = PS_SENSOR_ST_RESET;
  ps->temp = 0.0f;
  ps->pressure = 0.0f;
  memcpy(&ps->qx, qx, sizeof(ms5525dso_qx_t));
}

board_dev_status_t ps_update(board_dev_ps_t* ps) {
  hal_err_t res;

  // hal_log(HAL_LOG_DEBUG, "PS1", "State %u", ps->state);
  switch (ps->state) {
    case PS_SENSOR_ST_ERR:
      hal_log(HAL_LOG_ERROR, "PS1", "Failed");
      // Fall through to RESET
    case PS_SENSOR_ST_RESET:
      ps->ts_state = hal_get_timestamp();
      ps->status = BOARD_DEV_NOT_READY;
      res = ms5525dso_soft_reset(hal_i2c_get_config(ps->i2c_dev));
      ps->state = (res == HAL_OK) ? PS_SENSOR_ST_CONFIG : PS_SENSOR_ST_ERR;
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
                                           MS5525DSO_CH_D1_PRESSURE,
                                           MS5525DSO_OSR256);
          if (res == HAL_OK) {
            // If the conversion started, take a timestamp and prepare to read
            ps->ts_state = hal_get_timestamp();
            ps->state = PS_SENSOR_ST_READ_CH1;
          } else {
            ps->state = PS_SENSOR_ST_ERR;
          }
        } else {
          ps->state = PS_SENSOR_ST_ERR;
        }
      }
      break;

    case PS_SENSOR_ST_READ_CH1:
      // Has the previous conversion finished?
      if (hal_get_timestamp() >= (ps->ts_state + BOARD_PS_CONVERSION_TIME)) {
        res = ms5525dso_read_adc(hal_i2c_get_config(ps->i2c_dev), &ps->d1);
        if (res == HAL_OK) {
          res = ms5525dso_start_ch_convert(hal_i2c_get_config(ps->i2c_dev),
                                           MS5525DSO_CH_D2_TEMPERATURE,
                                           MS5525DSO_OSR256);
          if (res == HAL_OK) {
            // If the conversion started, take a timestamp and prepare to read
            ps->ts_state = hal_get_timestamp();
            ps->state = PS_SENSOR_ST_READ_CH2;
          } else {
            ps->state = PS_SENSOR_ST_ERR;
          }
        } else {
          ps->state = PS_SENSOR_ST_ERR;
        }
      }
      break;

    case PS_SENSOR_ST_READ_CH2:
      // Has the previous conversion finished?
      if (hal_get_timestamp() >= (ps->ts_state + BOARD_PS_CONVERSION_TIME)) {
        res = ms5525dso_read_adc(hal_i2c_get_config(ps->i2c_dev), &ps->d2);
        if (res == HAL_OK) {
          // Start the conversion again for channel 1
          res = ms5525dso_start_ch_convert(hal_i2c_get_config(ps->i2c_dev),
                                           MS5525DSO_CH_D1_PRESSURE,
                                           MS5525DSO_OSR256);
          if (res == HAL_OK) {
            ps->ts_state = hal_get_timestamp();
            ps->status = BOARD_DEV_READY;
            ps->state = PS_SENSOR_ST_READ_CH1;
            // Technically we could do this once the ADC read in this state
            // succeeds but there is no point if the next conversion start
            // failed, as the sensor state would go to SENSOR_STATUS_INIT so we
            // do it here instead
            ms5525dso_calculate_pt(&ps->qx, &ps->coeff, ps->d1, ps->d2,
                                   &ps->pressure, &ps->temp);
          } else {
            ps->state = PS_SENSOR_ST_ERR;
          }
        } else {
          ps->state = PS_SENSOR_ST_ERR;
        }
      }
      break;

    default:
      ps->status = BOARD_DEV_NOT_READY;
      ps->state = PS_SENSOR_ST_ERR;
      break;
  }

  return ps->status;
}

board_dev_status_t ps_get_status(board_dev_ps_t* ps) {
  assert(ps);
  if(!ps) {
    return BOARD_DEV_NOT_READY;
  }

  return ps->status;
}

board_dev_status_t ps_get_pressure(board_dev_ps_t* ps, float* pressure) {
  assert(ps);
  if(!ps) {
    return BOARD_DEV_NOT_READY;
  }

  *pressure = ps->pressure;

  return ps->status;
}

board_dev_status_t ps_get_temp(board_dev_ps_t* ps, float* temperature) {
  assert(ps);
  if(!ps) {
    return BOARD_DEV_NOT_READY;
  }

  *temperature = ps->temp;

  return ps->status;
}
