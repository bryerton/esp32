/*
Copyright 2020 MVM Project

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

#ifndef SFM3000_H
#define SFM3000_H

#include <hal_interface.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SFM3000
 * @ingroup driver
 * @brief I2C Driver for the Sensirion, SFM3000, 14-bit Low Pressure Drop
 * Digital Flow Meter
 * @{
 */

#define SFM3000_I2C_ADDR 0x40  //!< The SFM3000 has a fixed I2C address
#define SFM3000_REG_START_FLOW \
  0x1000  //!< Register to start the flow conversions
#define SFM3000_REG_START_TEMP \
  0x1001  //!< Register to start the temperature conversions
#define SFM3000_REG_SCALE_FACTOR \
  0x30DE  //!< Register to read the scale factor stored in the device
#define SFM3000_REG_OFFSET \
  0x30DF  //!< Register to read the offset stored in the SFM3000
#define SFM3000_REG_PRODUCT_HI \
  0x31E3  //!< Register to read the MSB of the Product ID
#define SFM3000_REG_PRODUCT_LO \
  0x31E4  //!< Register to read the LSB of the Product ID
#define SFM3000_REG_SERIAL_HI \
  0x31AE  //!< Register to read the MSB of the Serial Number
#define SFM3000_REG_SERIAL_LO \
  0x31AF  //!< Register to read the LSB of the Serial Number
#define SFM3000_REG_SOFT_RESET \
  0x2000  //!< Register to perform a soft reset of the device

#define SFM3000_STARTUP_TIME_MS \
  100  //!< Minimum reset time required to wait before communication after power
       //!< reset
#define SFM3000_SOFT_RESET_TIME_MS \
  80  //!< Minimum reset time required to wait before communication after soft
      //!< reset occurs
#define SFM3000_GIVEN_OFFSET \
  32000  //!< Given by datasheet, should match what's read from device
#define SFM3000_GIVEN_SCALE_FACTOR_AIR_N2 \
  140.0f  //!< Given by datasheet, used to adjust conversion calculation for
          //!< Air/N2
#define SFM3000_GIVEN_SCALE_FACTOR_O2 \
  142.8f  //!< Given by datasheet, used to adjust conversion calculation for O2
#define SFM3000_CONVERT_REG_TO_BUFF(b, r) \
  b[0] = (r) >> 8;                        \
  b[1] = (r)&0xFF  //!< Simple macro to help setup command word for i2c write

/** @brief Parameter settings for SFM3000
 *
 * Contains settings for calculations
 */
typedef struct sfm3000_settings_t {
  float offset;  //!< Offset to apply to calculation, defaults to 32000 for this
                 //!< device
  float scale_factor;  //!< Scale factor to use, defaults to 140, but should be
                       //!< changed if not using Air/N2
} sfm3000_settings_t;

/** @brief Soft reset of SFM3000
 *
 * Perform a software reset of the SFM3000 device, afterwards it will be
 * unavailable for ~80ms (per datasheet). Care must be taken to not use the
 * other functions of this device until the reset period is over
 *
 * @param cfg I2C configuration for this device
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_soft_reset(const hal_i2c_config_t* cfg);

/** @brief Start flow conversions
 *
 * Initiates automatic conversions of the flow measurements, which are performed
 * every ~0.5ms. Once this is called, the flow rate can be read out without
 * further calls to start conversions until another register is written to.
 *
 * @param cfg I2C configuration for this device
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_start_flow(const hal_i2c_config_t* cfg);

/** @brief Start temperature conversions
 *
 * Initiates automatic conversions of the temperature measurements, which are
 * performed every ~0.5ms. Once this is called, the temperature can be read out
 * without further calls to start conversions until another register is written
 * to.
 *
 * @param cfg I2C configuration for this device
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_start_temp(const hal_i2c_config_t* cfg);

/** @brief Read flow measurements
 *
 * Reads back the flow rate from the device.
 * The function then performs the conversion to standard litre per minute (slm)
 * given the offset and scale factor provided. The formula given for conversion
 * is \f{eqnarray*}{\mbox{flow [slm]} &=& \frac{\mbox{measured value} -
 * \mbox{offset flow}}{\mbox{scale factor flow}} \f}
 *
 * @param cfg I2C configuration for this device
 * @param settings Settings to use when performing the conversion from raw value
 * to standard litre per minute (slm)
 * @param flow Pointer to variable where flow rate value will be stored after
 * readback and conversion
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_flow(const hal_i2c_config_t* cfg,
                            sfm3000_settings_t* settings, float* flow);

/** @brief Read temperature measurements
 *
 * Reads back the temperature from the device.
 * The function then performs the conversion to Celsius.
 * WARNING: This formula is currently unknown
 *
 * @param cfg I2C configuration for this device
 * @param temp Pointer to variable where temperature value will be stored after
 * readback and conversion
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_temp(const hal_i2c_config_t* cfg, float* temp);

/** @brief Read scale factor from device
 *
 * Reads back the stored scale factor from the device. This (so far) has matched
 * the datasheets scale factor for Air / N2 readback of 140. It is not advisable
 * to use this scale factor if the gas flowing is not a match, for example if it
 * is O2. The datasheets gives recommendations on Air/N2 and O2, and it is
 * recommended to use one of those values as required, instead of pulling the
 * constant from the device. Device gives scale factor value as 16-bit unsigned
 * integer (MSB first), but function converts it to float
 *
 * @param cfg I2C configuration for this device
 * @param scale_factor Pointer to variable where scale_factor value will be
 * stored after readback
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_scale_factor(const hal_i2c_config_t* cfg,
                                    float* scale_factor);

/** @brief Read offset from device
 *
 * Reads back the stored offset from the device. This (so far) has matched the
 * datasheets offset of 32000. It may be advisable to use the stored value,
 * clarification from manufacturer might be advisable. Device gives offset value
 * as 16-bit unsigned integer (MSB first), but function converts it to float
 *
 * @param cfg I2C configuration for this device
 * @param offset Pointer to variable where offset value will be stored after
 * readback
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_offset(const hal_i2c_config_t* cfg, float* offset);

/** @brief Read serial number from device
 *
 * Reads back the stored serial from the device. Serial number is stored as
 * 32-bit unsigned integer with the MSB read out first.
 *
 * @param cfg I2C configuration for this device
 * @param serial Pointer to variable where serial number will be stored after
 * readback
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_serial(const hal_i2c_config_t* cfg, uint32_t* serial);

/** @brief Read product number from device
 *
 * Reads back the stored product number from the device. Product number is
 * stored as 32-bit unsigned integer with the MSB read out first.
 *
 * @param cfg I2C configuration for this device
 * @param product Pointer to variable where product number will be stored after
 * readback
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t sfm3000_read_product(const hal_i2c_config_t* cfg, uint32_t* product);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // SFM3000_H
