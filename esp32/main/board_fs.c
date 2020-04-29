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
#include <board_fs.h>
#include <drv_i2c_sfm3000.h>

static void update_state(board_dev_fs_t* fs, flow_sensor_state_t new_state);

void fs_init(board_dev_fs_t* fs, hal_i2c_dev_t i2c_dev,
             const sfm3000_settings_t* settings) {
  assert(fs);
  assert(settings);

  if ((fs != NULL) || (settings != NULL)) {
    fs->status = BOARD_DEV_NOT_READY;
    fs->i2c_dev = i2c_dev;
    fs->flow_raw = 0;
    fs->flow = 0.0f;
    fs->product = 0;
    fs->serial = 0;
    fs->settings.offset = SFM3000_GIVEN_OFFSET;
    fs->settings.scale_factor = SFM3000_GIVEN_SCALE_FACTOR_O2;
    update_state(fs, FS_SENSOR_ST_RESET);
  }
}

void fs_set_settings(board_dev_fs_t* fs, const sfm3000_settings_t* settings) {
  assert(fs);
  assert(settings);

  if ((fs != NULL) && (settings != NULL)) {
    memcpy(&fs->settings, settings, sizeof(sfm3000_settings_t));
  }
}

board_dev_status_t fs_update(board_dev_fs_t* fs) {
  hal_err_t res;
  board_dev_status_t retval;

  assert(fs);

  retval = BOARD_DEV_NOT_READY;

  if (fs != NULL) {
    switch (fs->state) {
      case FS_SENSOR_ST_RESET:
        fs->status = BOARD_DEV_NOT_READY;
        res = sfm3000_soft_reset(hal_i2c_get_config(fs->i2c_dev));
        if (res == HAL_OK) {
          update_state(fs, FS_SENSOR_ST_CONFIG);
        }
        break;

      case FS_SENSOR_ST_CONFIG:
        if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_RESET_TIME)) {
          res = sfm3000_read_product(hal_i2c_get_config(fs->i2c_dev),
                                     &fs->product);

          if (res == HAL_OK) {
            res = sfm3000_read_serial(hal_i2c_get_config(fs->i2c_dev),
                                      &fs->serial);
          }

          if (res == HAL_OK) {
            res = sfm3000_start_flow(hal_i2c_get_config(fs->i2c_dev));
          }

          if (res == HAL_OK) {
            hal_log(HAL_LOG_INFO, "FS1", "Product 0x%.08X", fs->product);
            hal_log(HAL_LOG_INFO, "FS1", "Serial 0x%.08X", fs->serial);
            update_state(fs, FS_SENSOR_ST_DISCARD_FIRST_FLOW);
          } else {
            update_state(fs, FS_SENSOR_ST_RESET);
          }
        }
        break;

      case FS_SENSOR_ST_DISCARD_FIRST_FLOW:
        // Has the previous conversion finished?
        // @NOTE: This is unexpected, and not in the datasheet, we must wait
        // significant time before the first flow reading, or all readings fail
        if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_RESET_TIME)) {
          // Don't check the result of the first flow reading, just move on to
          // reading real flow values
          sfm3000_read_flow(hal_i2c_get_config(fs->i2c_dev), &fs->flow_raw);
          update_state(fs, FS_SENSOR_ST_READ_FLOW);
        }
        break;

      case FS_SENSOR_ST_READ_FLOW:
        // Has the previous conversion finished?
        if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_CONVERSION_TIME)) {
          res =
              sfm3000_read_flow(hal_i2c_get_config(fs->i2c_dev), &fs->flow_raw);
          if (res == HAL_OK) {
            res =
                sfm3000_convert_to_slm(fs->flow_raw, &fs->settings, &fs->flow);
          }

          if (res == HAL_OK) {
            fs->status = BOARD_DEV_READY;
            update_state(fs, FS_SENSOR_ST_READ_FLOW);
          } else {
            update_state(fs, FS_SENSOR_ST_RESET);
          }
        }
        break;

      default:
        fs->status = BOARD_DEV_NOT_READY;
        update_state(fs, FS_SENSOR_ST_RESET);
        fs->state = FS_SENSOR_ST_RESET;
        break;
    }
    retval = fs->status;
  }

  return retval;
}

board_dev_status_t fs_get_flow(board_dev_fs_t* fs, float* flow) {
  board_dev_status_t res;

  assert(fs);
  assert(flow);

  res = BOARD_DEV_NOT_READY;

  if ((fs != NULL) && (flow != NULL)) {
    *flow = fs->flow;
    res = fs->status;
  }

  return res;
}

board_dev_status_t fs_get_status(board_dev_fs_t* fs) {
  board_dev_status_t res;

  assert(fs);

  res = BOARD_DEV_NOT_READY;

  if (fs != NULL) {
    res = fs->status;
  }

  return res;
}

static void update_state(board_dev_fs_t* fs, flow_sensor_state_t new_state) {
  assert(fs);

  if (fs != NULL) {
    fs->state = new_state;
    fs->ts_state = hal_get_timestamp();
  }
}
