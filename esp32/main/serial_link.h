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

#ifndef ESP32_MAIN_SERIAL_LINK_H_
#define ESP32_MAIN_SERIAL_LINK_H_

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <hal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SERIAL_LINK_RX_BUFF_LEN 512u
#define SERIAL_LINK_MAX_CMD_LEN 32u

typedef struct serial_link_t {
  uint8_t rx_buffer[SERIAL_LINK_RX_BUFF_LEN];
  uint32_t rx_len;
  QueueHandle_t event_queue;
} serial_link_t;

void serial_link_init(serial_link_t* link);

void serial_link_update(serial_link_t* link);

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_SERIAL_LINK_H_
