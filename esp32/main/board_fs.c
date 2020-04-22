#include <string.h>
#include <hal.h>
#include <board.h>
#include <board_fs.h>
#include <drv_i2c_sfm3000.h>

void fs_init(board_dev_fs_t* fs, hal_i2c_dev_t i2c_dev,
             const sfm3000_settings_t* settings) {
  assert(fs && settings);
  if (!fs || !settings) {
    return;
  }

  fs->status = BOARD_DEV_NOT_READY;
  fs->ts_state = 0;
  fs->i2c_dev = i2c_dev;
  fs->state = FS_SENSOR_ST_RESET;
  fs->flow_raw = 0;
  fs->flow = 0.0f;
  fs->product = 0;
  fs->serial = 0;
  fs->settings.offset = SFM3000_GIVEN_OFFSET;
  fs->settings.scale_factor = SFM3000_GIVEN_SCALE_FACTOR_O2;
}

void fs_set_settings(board_dev_fs_t* fs, const sfm3000_settings_t* settings) {
  assert(fs);
  if (!fs) {
    return;
  }

  memcpy(&fs->settings, settings, sizeof(sfm3000_settings_t));
}

board_dev_status_t fs_update(board_dev_fs_t* fs) {
  hal_err_t res;

  assert(fs);
  if (!fs) {
    return BOARD_DEV_NOT_READY;
  }

  switch (fs->state) {
    case FS_SENSOR_ST_ERR:
      hal_log(HAL_LOG_ERROR, "FS1", "Failed");
      // Fall through to RESET
    case FS_SENSOR_ST_RESET:
      fs->status = BOARD_DEV_NOT_READY;
      res = sfm3000_soft_reset(hal_i2c_get_config(fs->i2c_dev));
      if (res == HAL_OK) {
        fs->ts_state = hal_get_timestamp();
        fs->state = FS_SENSOR_ST_CONFIG;
      } else {
        fs->state = FS_SENSOR_ST_ERR;
      }
      break;

    case FS_SENSOR_ST_CONFIG:
      if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_RESET_TIME)) {

        res =
            sfm3000_read_product(hal_i2c_get_config(fs->i2c_dev), &fs->product);
        if (res == HAL_OK) {
          res =
              sfm3000_read_serial(hal_i2c_get_config(fs->i2c_dev), &fs->serial);
          if (res == HAL_OK) {
            res = sfm3000_start_flow(hal_i2c_get_config(fs->i2c_dev));
          }
        }

        if (res == HAL_OK) {
          fs->ts_state = hal_get_timestamp();
          hal_log(HAL_LOG_INFO, "FS1", "Product 0x%.08X", fs->product);
          hal_log(HAL_LOG_INFO, "FS1", "Serial 0x%.08X", fs->serial);
          fs->state = FS_SENSOR_ST_DISCARD_FIRST_FLOW;
        } else {
          fs->state = FS_SENSOR_ST_ERR;
        }
      }
      break;

    case FS_SENSOR_ST_DISCARD_FIRST_FLOW:
      // Has the previous conversion finished?
      // @NOTE: This is unexpected, and not in the datasheet, we must wait significant
      // time before the first flow reading, or all readings fail
      if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_RESET_TIME)) {
        // Don't check the result of the first flow reading, just move on to reading
        // real flow values
        sfm3000_read_flow(hal_i2c_get_config(fs->i2c_dev), &fs->flow_raw);
        fs->ts_state = hal_get_timestamp();
        fs->state = FS_SENSOR_ST_READ_FLOW;
      }
      break;

    case FS_SENSOR_ST_READ_FLOW:
      // Has the previous conversion finished?
      if (hal_get_timestamp() >= (fs->ts_state + BOARD_FS_CONVERSION_TIME)) {
        res = sfm3000_read_flow(hal_i2c_get_config(fs->i2c_dev), &fs->flow_raw);
        if (res == HAL_OK) {
          sfm3000_convert_to_slm(fs->flow_raw, &fs->settings, &fs->flow);
          fs->status = BOARD_DEV_READY;
          fs->ts_state = hal_get_timestamp();
          fs->state = FS_SENSOR_ST_READ_FLOW;
        } else {
          fs->state = FS_SENSOR_ST_ERR;
        }
      }
      break;

    default:
      fs->status = BOARD_DEV_NOT_READY;
      fs->state = FS_SENSOR_ST_ERR;
      break;
  }

  return fs->status;
}

board_dev_status_t fs_get_flow(board_dev_fs_t* fs, float* flow) {
  assert(fs);
  if (!fs) {
    return BOARD_DEV_NOT_READY;
  }

  if (fs->status == BOARD_DEV_READY) {
    *flow = fs->flow;
  } else {
    *flow = 0.0f;
  }

  return fs->status;
}

board_dev_status_t fs_get_status(board_dev_fs_t* fs) {
  assert(fs);
  if (!fs) {
    return BOARD_DEV_NOT_READY;
  }

  return fs->status;
}
