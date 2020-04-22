#include <string.h>
#include <stdarg.h>
#include <esp_system.h>
#include <hal.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <drv_i2c_ms5525dso.h>
#include <drv_i2c_sfm3000.h>
#include <drv_i2c_tca9548a.h>

static hal_log_level_t current_log_level = HAL_LOG_NONE;

static const char* get_log_color(hal_log_level_t log_level);
static const char* get_log_level_string(hal_log_level_t log_level);

hal_timestamp_t hal_get_timestamp(void) { return esp_timer_get_time(); }
void hal_set_log_level(hal_log_level_t new_log_level) {
  current_log_level = new_log_level;
}

hal_log_level_t hal_get_log_level(void) { return current_log_level; }

void hal_init(void) {
  const gpio_config_t io_output_cfg = {
      .intr_type = GPIO_PIN_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = ((uint64_t)1 << HAL_GPIO_DRV_RSTn_PIN) |
                      ((uint64_t)1 << HAL_GPIO_DRV_CH1_PIN) |
                      ((uint64_t)1 << HAL_GPIO_DRV_CH2_PIN) |
                      ((uint64_t)1 << HAL_GPIO_DRV_CH3_PIN) |
                      ((uint64_t)1 << HAL_GPIO_DRV_CH4_PIN) |
                      ((uint64_t)1 << HAL_GPIO_LED1_PIN) |
                      ((uint64_t)1 << HAL_GPIO_LED2_PIN),
      .pull_down_en = 0,
      .pull_up_en = 0};

  ESP_ERROR_CHECK(gpio_config(&io_output_cfg));

  const i2c_config_t i2c_cfg = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = HAL_I2C_MASTER_SDA_IO_PIN,
      .sda_pullup_en = false,
      .scl_io_num = HAL_I2C_MASTER_SCL_IO_PIN,
      .scl_pullup_en = false,
      .master.clk_speed = HAL_I2C_MASTER_FREQ,
  };

  ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_cfg));
  ESP_ERROR_CHECK(
      i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM));

  ESP_ERROR_CHECK(i2c_set_timeout(I2C_NUM_0, HAL_I2C_DEFAULT_TIMEOUT_PERIOD));
}


void hal_log(hal_log_level_t log_level, const char* topic, const char* fmt,
             ...) {
  char str_log[HAL_LOG_MAX_LEN];

  snprintf(str_log, HAL_LOG_MAX_LEN, "%s%.1s (%llu) %.8s: ",
           get_log_color(log_level), get_log_level_string(log_level),
           esp_timer_get_time(), topic);

  va_list args;
  va_start(args, fmt);
  vsnprintf(&str_log[strnlen(str_log, HAL_LOG_MAX_LEN)], HAL_LOG_MAX_LEN, fmt,
            args);
  va_end(args);

  printf("%s\n", str_log);
}

static const char* get_log_level_string(hal_log_level_t log_level) {
  switch (log_level) {
    case HAL_LOG_ERROR:
      return "E";
    case HAL_LOG_WARN:
      return "W";
    case HAL_LOG_INFO:
      return "I";
    case HAL_LOG_DEBUG:
      return "D";
    default:
      return 0;
  }
}

static const char* get_log_color(hal_log_level_t log_level) {
  switch (log_level) {
    case HAL_LOG_ERROR:
      return "\033[0;31m";
    case HAL_LOG_WARN:
      return "\033[0;33m";
    case HAL_LOG_INFO:
      return "\033[0;32m";
    case HAL_LOG_DEBUG:
      return "\033[0;35m";
    default:
      return 0;
  }
}

void hal_gpio_write(hal_gpio_t pin, int value) { gpio_set_level(pin, value); }

int hal_gpio_read(hal_gpio_t pin) { return gpio_get_level(pin); }

static const hal_i2c_config_t i2c_switch_cfg = {
    .i2c_addr = HAL_I2C_SWITCH_ADDR,
    .i2c_port_num = I2C_NUM_0,
    .i2c_timeout = HAL_I2C_DEFAULT_TIMEOUT_PERIOD};

static const hal_i2c_config_t i2c_ps_cfg = {
    // PS1 Config
    .i2c_addr = HAL_I2C_PS1_ADDR,
    .i2c_port_num = I2C_NUM_0,
    .i2c_timeout = HAL_I2C_DEFAULT_TIMEOUT_PERIOD};

static const hal_i2c_config_t i2c_fs_cfg = {
    // FS1 Config
    .i2c_addr = HAL_I2C_FS1_ADDR,
    .i2c_port_num = I2C_NUM_0,
    .i2c_timeout = HAL_I2C_DEFAULT_TIMEOUT_PERIOD};

const hal_i2c_config_t* hal_i2c_get_config(hal_i2c_dev_t dev) {
  switch (dev) {
    case HAL_I2C_DEV_SWITCH:
      return &i2c_switch_cfg;
    case HAL_I2C_DEV_PS1:
      return &i2c_ps_cfg;
    case HAL_I2C_DEV_FS1:
      return &i2c_fs_cfg;
    default:
      return 0;
  }

  return 0;
}

hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len) {
  i2c_cmd_handle_t cmd;
  esp_err_t res;
  uint8_t n;

  // Create link to queue i2c messages up into
  cmd = i2c_cmd_link_create();
  if (!cmd) {
    return HAL_ERR_FAIL;
  }

  // Queue up messages, if the arguments are wrong, lets assert immediately
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(
      i2c_master_write_byte(cmd, (cfg->i2c_addr << 1) | I2C_MASTER_WRITE, 1));
  for (n = 0; n < len; n++) {
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, buffer[n], 1));
  }
  ESP_ERROR_CHECK(i2c_master_stop(cmd));

  // Execute queued i2c commands
  res = i2c_master_cmd_begin(cfg->i2c_port_num, cmd, cfg->i2c_timeout);

  // Cleanup the link
  i2c_cmd_link_delete(cmd);

  return (res == ESP_OK) ? HAL_OK : HAL_ERR_FAIL;
}

hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len) {
  i2c_cmd_handle_t cmd;
  esp_err_t res;
  uint8_t n;

  assert(cfg);
  assert(buffer);
  assert(len);
  if ((!cfg) || (!buffer) || (!len)) {
    return HAL_ERR_FAIL;
  }

  // Create link to queue i2c messages up into
  cmd = i2c_cmd_link_create();
  if (!cmd) {
    return HAL_ERR_FAIL;
  }

  // Queue up messages, if the arguments are wrong, lets assert immediately
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(
      i2c_master_write_byte(cmd, (cfg->i2c_addr << 1) | I2C_MASTER_READ, 1));
  for (n = 0; n < len; n++) {
    ESP_ERROR_CHECK(i2c_master_read_byte(
        cmd, &buffer[n], (n < (len - 1)) ? I2C_MASTER_ACK : I2C_MASTER_NACK));
  }
  ESP_ERROR_CHECK(i2c_master_stop(cmd));

  // Execute queued i2c commands
  res = i2c_master_cmd_begin(cfg->i2c_port_num, cmd, cfg->i2c_timeout);

  // Cleanup the link
  i2c_cmd_link_delete(cmd);

  return (res == ESP_OK) ? HAL_OK : HAL_ERR_FAIL;
}
