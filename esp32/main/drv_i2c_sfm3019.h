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

#ifndef ESP32_MAIN_DRV_I2C_SFM3019_H_
#define ESP32_MAIN_DRV_I2C_SFM3019_H_

#include <hal.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SFM3019 SFM3019
 * @ingroup driver_i2c
 * @brief I2C Driver for the Sensirion, SFM3019, 14-bit Low Pressure Drop
 * Digital Flow Meter
 * @{
 */

/** SFM3019 has a fixed I2C address */
#define SFM3019_I2C_ADDR 0x2Eu

/** Soft reset command is 8-bit */
#define SFM3019_REG_SOFT_RESET 0x06u

/** O2 gas command code */
#define SFM3019_REG_GAS_O2 0x3603u

/** Air gas command code */
#define SFM3019_REG_GAS_AIR 0x3608u

/** Mixed gas command code */
#define SFM3019_REG_GAS_MIX 0x3632u

/** Update concentration command code */
#define SFM3019_REG_UPDATE_CONC 0xE17Du

/** I2C Reset Address Pointer command code
 *
 * Only used in combination with Update Concentration command
 */
#define SFM3019_REG_RESET_PTR 0xE000u

/** Stop continous measurement command code
 *
 * Must be used before any other command, if continuous measurements are ongoing
 */
#define SFM3019_REG_STOP_MEAS 0x3FF9u

/** Continous averaging mode command code */
#define SFM3019_REG_CONF_AVG 0x366Au

/** Readout of specific gas lookup table scale, offset, and flow unit command
 * code */
#define SFM3019_REG_READ_SETTINGS 0x3661u

/** Read product number and serial number command code */
#define SFM3019_REG_READ_PRODUCT 0xE102u

typedef enum sfm3019_gas_t {
  SFM3019_GAS_O2 = 0,  //!< Pure O2 gas mix
  SFM3019_GAS_AIR,     //!< Air gas mix (20% Oxygen?)
  SFM3019_GAS_MIX      //!< Some fraction of Air / O2
} sfm3019_gas_t;

/** @brief Start flow conversions
 *
 * Initiates automatic conversions of the flow measurements, which are performed
 * every ~0.5ms. Once this is called, the flow rate can be read out without
 * further calls to start conversions until another register is written to.
 *
 * @param cfg I2C configuration for this device
 * @param gas Which gas is flowing
 * @param fraction Optional, if gas is set to MIX, what is the fraction of O2 in
 * % (0-1023)
 * @return hal_err_t
 */
hal_err_t sfm3019_start_cont_meas(const hal_i2c_config_t* cfg,
                                  sfm3019_gas_t gas, uint16_t fraction);

/**
 * @brief Read flow conversion out
 *
 * @param cfg I2C configuration for this device
 * @param flow Raw Flow rate
 * @param temp Raw Temperature
 * @param status Status word of device
 * @return hal_err_t
 */
hal_err_t sfm3019_read_meas(const hal_i2c_config_t* cfg, int16_t* flow,
                            int16_t* temp, int16_t* status);

/**
 * @brief Update concentration
 *
 * Updates the given gas concentration, can be used during continous measurement mode
 *
 * @param cfg I2C configuration for this device
 * @param fraction Fraction of O2 in mix in % (0-1023)
 * @return hal_err_t
 */
hal_err_t sfm3019_update_concentration(const hal_i2c_config_t* cfg,
                                       uint16_t fraction);

/**
 * @brief Stop continous measurement mode
 *
 * @param cfg I2C configuration for this device
 * @return hal_err_t
 */
hal_err_t sfm3019_stop_cont_meas(const hal_i2c_config_t* cfg);

/**
 * @brief Configure averaging mode
 *
 * @param cfg I2C configuration for this device
 * @param average Averaging mode selected, 0=average-until-read, 1-128=fixed-N averaging mode
 * @return hal_err_t
 */
hal_err_t sfm3019_configure_avg(const hal_i2c_config_t* cfg, uint16_t average);

/**
 * @brief Read lookup table values for given gas mix
 *
 * @param cfg I2C configuration for this device
 * @param gas Gas mix to lookup table for
 * @param scale_factor Filled with scale factor for given gas setting
 * @param offset Filled with offset for given gas setting
 * @param flow_unit Filled with flow_unit for given gas setting
 * @return hal_err_t
 */
hal_err_t sfm3019_read_settings(const hal_i2c_config_t* cfg, sfm3019_gas_t gas,
                                int16_t* scale_factor, int16_t* offset,
                                uint16_t* flow_unit);

/** @brief Read product identification numbers from device
 *
 * Reads back the stored product identification numbers from the device.
 * Product number is stored as 32-bit unsigned integer with the MSB read out
 * first. Serial number is stored as 64-bit unsigned integer with the MSB read
 * out first.
 *
 * @param cfg I2C configuration for this device
 * @param product Pointer to variable where product number will be stored
 * @param serial Pointer to variable where serial number will be stored
 * @return hal_err_t
 */
hal_err_t sfm3019_read_product_ident(const hal_i2c_config_t* cfg,
                                     uint32_t* product, uint64_t* serial);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // ESP32_MAIN_DRV_I2C_SFM3019_H_
