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

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <board.h>
#include <control.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <link.h>
#include <main.h>
#include <nvs_flash.h>

static StackType_t stackbuffer_control[TASK_CONTROL_STACK_SIZE];
static StaticTask_t taskbuffer_control;
static TaskHandle_t task_control_handle;

static StackType_t stackbuffer_board[TASK_BOARD_STACK_SIZE];
static StaticTask_t taskbuffer_board;
static TaskHandle_t task_board_handle;

static StackType_t stackbuffer_link[TASK_LINK_STACK_SIZE];
static StaticTask_t taskbuffer_link;
static TaskHandle_t task_link_handle;

static void task_control(void* param);
static void task_board(void* param);
static void task_link(void* param);

/**
 * @brief Entry function
 *
 */
void app_main(void) {
  esp_task_wdt_add(NULL);

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  hal_init();

  task_control_handle = xTaskCreateStaticPinnedToCore(
      &task_control, TASK_CONTROL_NAME, TASK_CONTROL_STACK_SIZE, NULL,
      TASK_CONTROL_PRIORITY, stackbuffer_control, &taskbuffer_control,
      TASK_CONTROL_PINNED_CORE);

  task_board_handle = xTaskCreateStaticPinnedToCore(
      &task_board, TASK_BOARD_NAME, TASK_BOARD_STACK_SIZE, NULL,
      TASK_BOARD_PRIORITY, stackbuffer_board, &taskbuffer_board,
      TASK_BOARD_PINNED_CORE);

  task_link_handle = xTaskCreateStaticPinnedToCore(
      &task_link, TASK_LINK_NAME, TASK_LINK_STACK_SIZE, NULL,
      TASK_LINK_PRIORITY, stackbuffer_link, &taskbuffer_link,
      TASK_LINK_PINNED_CORE);

  esp_task_wdt_delete(NULL);
}

static void task_control(void* param) {
  static int64_t last_time;
  TickType_t xLastWakeTime;
  int64_t current_time;

  control_init();

  xLastWakeTime = xTaskGetTickCount();
  last_time = esp_timer_get_time();
  for (;;) {
    current_time = hal_get_timestamp();
    control_update(current_time, current_time - last_time);
    last_time = current_time;

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_CONTROL_INTERVAL_MS));
  }
}

static void task_board(void* param) {
  TickType_t xLastWakeTime;
  board_t board;

  board_init(&board);

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    board_update(&board);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_BOARD_INTERVAL_MS));
  }
}

static void task_link(void* param) {
  static int64_t last_time;
  TickType_t xLastWakeTime;
  int64_t current_time;

  link_init();

  xLastWakeTime = xTaskGetTickCount();
  last_time = esp_timer_get_time();
  for (;;) {
    current_time = hal_get_timestamp();
    link_update(current_time, current_time - last_time);
    last_time = current_time;

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_LINK_INTERVAL_MS));
  }
}
