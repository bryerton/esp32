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

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <board.h>
#include <control.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <serial_link.h>
#include <nvs_flash.h>
#include "main.h"

static StackType_t stackbuffer_control[TASK_CONTROL_STACK_SIZE];
static StaticTask_t taskbuffer_control;
static TaskHandle_t task_control_handle;

static StackType_t stackbuffer_board[TASK_BOARD_STACK_SIZE];
static StaticTask_t taskbuffer_board;
static TaskHandle_t task_board_handle;

static StackType_t stackbuffer_serial_link[TASK_SERIAL_LINK_STACK_SIZE];
static StaticTask_t taskbuffer_serial_link;
static TaskHandle_t task_serial_link_handle;

static void task_control(void* param);
static void task_board(void* param);
static void task_serial_link(void* param);

/**
 * @brief Entry function
 *
 */
void app_main(void) {
  esp_task_wdt_add(NULL);

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if ((ret == ESP_ERR_NVS_NO_FREE_PAGES) ||
      (ret == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  hal_init();

  // Create the Controller task
  task_control_handle = xTaskCreateStaticPinnedToCore(
      &task_control, TASK_CONTROL_NAME, TASK_CONTROL_STACK_SIZE, NULL,
      TASK_CONTROL_PRIORITY, stackbuffer_control, &taskbuffer_control,
      TASK_CONTROL_PINNED_CORE);

  // Create the Board task
  task_board_handle = xTaskCreateStaticPinnedToCore(
      &task_board, TASK_BOARD_NAME, TASK_BOARD_STACK_SIZE, NULL,
      TASK_BOARD_PRIORITY, stackbuffer_board, &taskbuffer_board,
      TASK_BOARD_PINNED_CORE);

  // Create the Serial Link task
  task_serial_link_handle = xTaskCreateStaticPinnedToCore(
      &task_serial_link, TASK_SERIAL_LINK_NAME, TASK_SERIAL_LINK_STACK_SIZE, NULL,
      TASK_SERIAL_LINK_PRIORITY, stackbuffer_serial_link, &taskbuffer_serial_link,
      TASK_SERIAL_LINK_PINNED_CORE);

  esp_task_wdt_delete(NULL);
}

static void task_control(void* param) {
  TickType_t xLastWakeTime;
  control_t control;

  esp_task_wdt_add(task_control_handle);
  control_init(&control);

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    esp_task_wdt_reset();
    control_update(&control);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_CONTROL_INTERVAL_MS));
  }
  esp_task_wdt_delete(task_control_handle);
}

static void task_board(void* param) {
  TickType_t xLastWakeTime;
  board_t board;

  esp_task_wdt_add(task_board_handle);
  board_init(&board);

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    esp_task_wdt_reset();
    board_update(&board);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_BOARD_INTERVAL_MS));
  }
  esp_task_wdt_delete(task_board_handle);
}

static void task_serial_link(void* param) {
  TickType_t xLastWakeTime;
  serial_link_t serial_link;

  esp_task_wdt_add(task_serial_link_handle);
  serial_link_init(&serial_link);

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    esp_task_wdt_reset();
    serial_link_update(&serial_link);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_SERIAL_LINK_INTERVAL_MS));
  }
  esp_task_wdt_delete(task_serial_link_handle);
}
