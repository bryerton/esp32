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

#ifndef ESP32_MAIN_DRV_I2C_TCA9548A_H_
#define ESP32_MAIN_DRV_I2C_TCA9548A_H_

#include <hal.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup TCA9548A TCA9548A
 * @ingroup driver_i2c
 * @brief I2C Driver for the Texas Instruments, TCA9548A, Low-Voltage 8-Channel
 * I2C Switch with Reset
 * @{
 */

#define TCA9548A_ADDR_LLL 0x70  //!< Address option for A2/A1/A0 (000)
#define TCA9548A_ADDR_LLH 0x71  //!< Address option for A2/A1/A0 (001)
#define TCA9548A_ADDR_LHL 0x72  //!< Address option for A2/A1/A0 (010)
#define TCA9548A_ADDR_LHH 0x73  //!< Address option for A2/A1/A0 (011)
#define TCA9548A_ADDR_HLL 0x74  //!< Address option for A2/A1/A0 (100)
#define TCA9548A_ADDR_HLH 0x75  //!< Address option for A2/A1/A0 (101)
#define TCA9548A_ADDR_HHL 0x76  //!< Address option for A2/A1/A0 (110)
#define TCA9548A_ADDR_HHH 0x77  //!< Address option for A2/A1/A0 (111)

#define TCA9548A_CH0 (1u << 0)  //!< Channel 0 bitmask
#define TCA9548A_CH1 (1u << 1)  //!< Channel 1 bitmask
#define TCA9548A_CH2 (1u << 2)  //!< Channel 2 bitmask
#define TCA9548A_CH3 (1u << 3)  //!< Channel 3 bitmask
#define TCA9548A_CH4 (1u << 4)  //!< Channel 4 bitmask
#define TCA9548A_CH5 (1u << 5)  //!< Channel 5 bitmask
#define TCA9548A_CH6 (1u << 6)  //!< Channel 6 bitmask
#define TCA9548A_CH7 (1u << 7)  //!< Channel 7 bitmask

/** @brief Set the active channel(s) on the device
 *
 * @param cfg Configuration to use for the i2c communication to the TCA9548
 * @param ch_bitmask Bitmask of channel(s) to connect to the i2c bus
 * @return HAL_OK if no error
 */
hal_err_t tca9548a_write_channel(const hal_i2c_config_t* cfg,
                                 uint8_t ch_bitmask);

/** @brief Read the I2C switch channel
 *
 * @param cfg Configuration to use for the i2c communication to the TCA9548
 * @param ch_bitmask Pointer to variable to fill in with bitmask of channel(s)
 * currently switched
 * @return HAL_OK if no error
 */
hal_err_t tca9548a_read_channel(const hal_i2c_config_t* cfg,
                                uint8_t* ch_bitmask);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_DRV_I2C_TCA9548A_H_
