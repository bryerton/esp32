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
#include <serial_link.h>
#include <hal.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>

static const uint32_t EVENT_QUEUE_DEPTH = 8;
static const uint32_t TX_BUFFER_SZ = 256;
static const uint32_t RX_BUFFER_SZ = 256;

static const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

static void detect_text_command(serial_link_t* serial_link);
static void parse_cmd(const uint8_t* cmd, uint32_t cmd_len);

void serial_link_init(serial_link_t* serial_link) {
  assert(serial_link);

  if (serial_link != NULL) {
    serial_link->rx_len = 0;
    serial_link->event_queue =
        xQueueCreate(EVENT_QUEUE_DEPTH, sizeof(uart_event_t));
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, RX_BUFFER_SZ, TX_BUFFER_SZ,
                        EVENT_QUEUE_DEPTH, serial_link->event_queue, 0);
    uart_flush_input(UART_NUM_0);
  }
}

void serial_link_update(serial_link_t* serial_link) {
  assert(serial_link);

  if (serial_link != NULL) {
    detect_text_command(serial_link);
  }
}

static void detect_text_command(serial_link_t* serial_link) {
  size_t bytes_to_read;
  int bytes_read;

  // Determine how many bytes we can read
  uart_get_buffered_data_len(UART_NUM_0, &bytes_to_read);
  if (bytes_to_read > (SERIAL_LINK_RX_BUFF_LEN - serial_link->rx_len)) {
    // Going to overflow, shift down enough to add all the new bytes
    // Save on stack by re-using 'bytes_read' as number of bytes the incoming
    // data is over SERIAL_LINK_RX_BUFF_LEN max length
    bytes_read =
        (serial_link->rx_len + bytes_to_read) - SERIAL_LINK_RX_BUFF_LEN;
    memmove(serial_link->rx_buffer, &serial_link->rx_buffer[bytes_read],
            SERIAL_LINK_RX_BUFF_LEN - bytes_read);
    serial_link->rx_len = SERIAL_LINK_RX_BUFF_LEN - bytes_to_read;
  }

  // Read the new bytes from FIFO
  bytes_read =
      uart_read_bytes(UART_NUM_0, &serial_link->rx_buffer[serial_link->rx_len],
                      bytes_to_read, 0);
  if (bytes_read > 0) {
    serial_link->rx_len += bytes_read;
  }

  // Check if we have a full command now
  for (uint32_t n = 0; n < serial_link->rx_len; n++) {
    if (serial_link->rx_buffer[n] == '\r') {
      serial_link->rx_buffer[n] = '\0';

      // Found a command, and not just an empty string
      if (n > 0) {
        parse_cmd(&serial_link->rx_buffer[0], n);
      }

      // Shift everything after the command back down to start from pos 0.
      // make sure to skip ahead one to avoid putting the '\n' at the start
      // of the next command buffer
      if (n < (SERIAL_LINK_RX_BUFF_LEN - 1)) {
        memmove(serial_link->rx_buffer, &serial_link->rx_buffer[n + 1], n);
        serial_link->rx_len -= n + 1;
      } else {
        serial_link->rx_len = 0;
      }

      // Escape loop if command has been found
      break;
    }
  }
}

static void parse_cmd(const uint8_t* cmd, uint32_t cmd_len) {
  char* pos;

  assert(cmd);

  if (cmd != NULL) {
    // Guaranteed to have passed in a null terminated string from
    // serial_link_update()

    pos = strchr((char*)cmd, ' ');
    // Did we get to the end of the string
    if (pos == '\0') {
    }
  }
}
