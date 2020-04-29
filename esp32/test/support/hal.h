#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

typedef enum hal_err_t { HAL_OK, HAL_ERR_FAIL } hal_err_t;

typedef struct hal_i2c_config_t {
  uint8_t i2c_addr;
} hal_i2c_config_t;

hal_err_t hal_i2c_write(const hal_i2c_config_t* cfg, const uint8_t* buffer,
                        uint8_t len);

hal_err_t hal_i2c_read(const hal_i2c_config_t* cfg, uint8_t* buffer,
                       uint8_t len);

#endif
