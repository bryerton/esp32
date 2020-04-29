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

#ifndef ESP32_MAIN_CONTROL_H_
#define ESP32_MAIN_CONTROL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup control Controller
 * @brief Controller for the device
 * @{
 */

typedef enum control_mode_t {
  CONTROL_MODE_OFF,
  CONTROL_MODE_MANUAL,
} control_mode_t;

typedef enum control_state_t {
  CONTROL_STATE_RESET,
  CONTROL_STATE_INIT,
  CONTROL_STATE_START,
  CONTROL_STATE_RUN,
  CONTROL_STATE_STOP,
  CONTROL_STATE_SHUTDOWN
} control_state_t;

typedef struct control_settings_t {
  control_mode_t mode;
} control_settings_t;

typedef struct control_t {
  control_state_t state;
  control_settings_t settings;
} control_t;

void control_init(control_t* control);

void control_update(control_t* control);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_CONTROL_H_
