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

#include <board.h>
#include <board_sw.h>
#include <board_ps.h>
#include <board_fs.h>
#include <hal.h>
#include <drv_i2c_ms5525dso.h>
#include <drv_i2c_sfm3000.h>
#include <drv_i2c_tca9548a.h>

static const ms5525dso_qx_t common_ps_qx = MS5525DSO_QX_FOR_PP001DS();

static sfm3000_settings_t fs_settings = {
    .offset = SFM3000_GIVEN_OFFSET,
    .scale_factor = SFM3000_GIVEN_SCALE_FACTOR_O2};

void board_init(board_t* board) {
  assert(board);
  if (!board) {
    return;
  }

  board->ts_state = 0;
  board->state = BOARD_ST_HARD_RESET;
  sw_init(&board->sw, HAL_I2C_DEV_SWITCH);
  ps_init(&board->ps1, HAL_I2C_DEV_PS1, &common_ps_qx);
  fs_init(&board->fs1, HAL_I2C_DEV_FS1, &fs_settings);
}

void board_update(board_t* board) {
  assert(board);
  if (!board) {
    return;
  }

  // hal_log(HAL_LOG_DEBUG, "BOARD", "State: %u", board->state);
  switch (board->state) {
    case BOARD_ST_HARD_RESET:
      hal_log(HAL_LOG_INFO, "BOARD", "Hard reset occurred");
      board->ts_state = hal_get_timestamp();
      hal_gpio_write(HAL_GPIO_DRV_RSTn_PIN, 0);
      board->state = BOARD_ST_HARD_RESET_WAIT;
      break;

    case BOARD_ST_HARD_RESET_WAIT:
      if (hal_get_timestamp() > board->ts_state + BOARD_HARD_RESET_TIME) {
        hal_gpio_write(HAL_GPIO_DRV_RSTn_PIN, 1);
        board->state = BOARD_ST_SOFT_RESET;
      }
      break;

    case BOARD_ST_SOFT_RESET:
      hal_log(HAL_LOG_INFO, "BOARD", "Soft reset occurred");
      // Mark the start of this state
      board->ts_state = hal_get_timestamp();

      // (re)Initialize sensor controllers
      sw_init(&board->sw, HAL_I2C_DEV_SWITCH);
      ps_init(&board->ps1, HAL_I2C_DEV_PS1, &common_ps_qx);
      fs_init(&board->fs1, HAL_I2C_DEV_FS1, &fs_settings);

      // Start updating the pressure sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_PS1);
      ps_update(&board->ps1);

      // Start updating the flow sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_FS1);
      fs_update(&board->fs1);

      board->state = BOARD_ST_SOFT_RESET_WAIT;
      break;

    case BOARD_ST_SOFT_RESET_WAIT:
      // Update the presssure sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_PS1);
      ps_update(&board->ps1);

      // Update the flow sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_FS1);
      fs_update(&board->fs1);

      // Keep trying until both boards are ready, or we timeout and reset
      if ((ps_get_status(&board->ps1) == BOARD_DEV_READY) &&
          (fs_get_status(&board->fs1) == BOARD_DEV_READY)) {
        board->state = BOARD_ST_RUNNING;
      } else if(hal_get_timestamp() > (board->ts_state + BOARD_SOFT_RESET_TIMEOUT)) {
        board->state = BOARD_ST_HARD_RESET;
      }
      break;

    case BOARD_ST_RUNNING:
      // Update the pressure sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_PS1);
      ps_update(&board->ps1);

      // Update the flow sensor
      sw_set_channel(&board->sw, HAL_I2C_SWITCH_CH_FS1);
      fs_update(&board->fs1);

      // If either device errors, we are in trouble
      if ((ps_get_status(&board->ps1) != BOARD_DEV_READY) ||
          (fs_get_status(&board->fs1) != BOARD_DEV_READY)) {
        board->state = BOARD_ST_HARD_RESET;
      }
      break;

    default:
      // This should never happen
      hal_log(HAL_LOG_ERROR, "BOARD", "Unknown board state %u", board->state);

      // Reset the board (not the device) and attempt recovery
      board->state = BOARD_ST_HARD_RESET;
      break;
  }
}
