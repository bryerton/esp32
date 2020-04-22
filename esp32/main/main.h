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

#ifndef ESP32_MAIN_MAIN_H_
#define ESP32_MAIN_MAIN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_CONTROL_STACK_SIZE 8192
#define TASK_CONTROL_PRIORITY 8
#define TASK_CONTROL_INTERVAL_MS 10
#define TASK_CONTROL_PINNED_CORE 1
#define TASK_CONTROL_NAME "control"

#define TASK_BOARD_STACK_SIZE 8192
#define TASK_BOARD_PRIORITY 7
#define TASK_BOARD_INTERVAL_MS 5
#define TASK_BOARD_PINNED_CORE 0
#define TASK_BOARD_NAME "board"

#define TASK_LINK_STACK_SIZE 8192
#define TASK_LINK_PRIORITY 6
#define TASK_LINK_INTERVAL_MS 10
#define TASK_LINK_PINNED_CORE 0
#define TASK_LINK_NAME "link"

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_MAIN_H_
