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

#ifndef DRV_MS5525DSO_H
#define DRV_MS5525DSO_H

#include <hal_interface.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MS5525DSO
 * @ingroup driver
 * @brief I2C Driver for the TE Connectivity, MS5525DSO, 24-bit integrated
 * digital presssure sensor
 * @{
 */

#define MS5525DSO_I2C_ADDR_LOW \
  0x77  //!< If CSB (CSn) pin is low, the MS5525DSO has this I2C address
#define MS5525DSO_I2C_ADDR_HIGH \
  0x76  //!< If CSB (CSn) pin is high, the MS5525DSO has this I2C address

#define MS5525DSO_REG_RESET \
  0x1E  //!< Reset register is required to be selected prior to normal operation
#define MS5525DSO_REG_ADC_READ \
  0x00  //!< After a convert request to D1 or D2 is done, write to this register
        //!< to request readback
#define MS5525DSO_REG_PROM_READ_BASE \
  0xA0  //!< PROM Address range is 0xA0 to 0xAE
#define MS5525DSO_REG_PROM_ADDR_MASK 0x7  //!< Mask of the PROM address bits
#define MS5525DSO_REG_PROM_ADDR_OFST 1    //!< Offset of PROM address bits

#define MS5525DSO_NUM_PROM_BYTES \
  2  //!< Number of PROM bytes per address location. Stored MSB first
#define MS5525DSO_NUM_ADC_BYTES \
  3  //!< Number of bytes to readback for ADC communication for D1 and D2
#define MS5525DSO_NUM_PROM_ADDR 8  //!< Number of PROM address registers

#define MS5525DSO_CONVERT_P_TO_FLOAT(x) \
  ((float)(x) /                         \
   10000.0f)  //!< Utility function to convert compensated pressure to PSI
#define MS5525DSO_CONVERT_T_TO_FLOAT(x) \
  ((float)(x) / 100.0f)  //!< Utility function to convert compensated
                         //!< temperature to Celsius

// Qx Coefficients Matrix by Pressure Range
#define MS5525DSO_QX_FOR_PP001DS() \
  { .Q1 = 15, .Q2 = 17, .Q3 = 7, .Q4 = 5, .Q5 = 7, .Q6 = 21 }
#define MS5525DSO_QX_FOR_PP002GS() \
  { .Q1 = 14, .Q2 = 16, .Q3 = 8, .Q4 = 6, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP002DS() \
  { .Q1 = 16, .Q2 = 18, .Q3 = 6, .Q4 = 4, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP005GS() \
  { .Q1 = 16, .Q2 = 17, .Q3 = 6, .Q4 = 5, .Q5 = 7, .Q6 = 21 }
#define MS5525DSO_QX_FOR_PP005DS() \
  { .Q1 = 17, .Q2 = 19, .Q3 = 5, .Q4 = 3, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP015GS() \
  { .Q1 = 16, .Q2 = 17, .Q3 = 6, .Q4 = 5, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP015AS() \
  { .Q1 = 16, .Q2 = 17, .Q3 = 6, .Q4 = 5, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP015DS() \
  { .Q1 = 17, .Q2 = 19, .Q3 = 5, .Q4 = 3, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP030AS() \
  { .Q1 = 17, .Q2 = 18, .Q3 = 5, .Q4 = 4, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP030GS() \
  { .Q1 = 17, .Q2 = 18, .Q3 = 5, .Q4 = 4, .Q5 = 7, .Q6 = 22 }
#define MS5525DSO_QX_FOR_PP030DS() \
  { .Q1 = 18, .Q2 = 21, .Q3 = 4, .Q4 = 1, .Q5 = 7, .Q6 = 22 }

/** @brief Channels that can be selected for conversion
 */
typedef enum ms5525dso_ch_e {
  MS5525DSO_CH_D1_PRESSURE,
  MS5525DSO_CH_D2_TEMPERATURE
} ms5525dso_ch_e;

/** @brief Oversample rates that can be selected for conversion
 */
typedef enum ms5525dso_osr_e {
  MS5525DSO_OSR256,
  MS5525DSO_OSR512,
  MS5525DSO_OSR1024,
  MS5525DSO_OSR2048,
  MS5525DSO_OSR4096
} ms5525dso_osr_e;

/** @brief Coefficient table for storage of PROM values
 */
typedef struct ms5525dso_coeff_t {
  uint16_t c[MS5525DSO_NUM_PROM_ADDR];
} ms5525dso_coeff_t;

/** @brief Qx coefficient table
 */
typedef struct ms5525dso_qx_t {
  int8_t Q1;
  int8_t Q2;
  int8_t Q3;
  int8_t Q4;
  int8_t Q5;
  int8_t Q6;
} ms5525dso_qx_t;

/** @brief Perform soft reset of device
 *
 * Performs a soft reset of the device, this required on power-on-reset. Per
 * datasheet "The Reset sequence shall be sent once after power-on to make sure
 * that the calibration PROM gets loaded into the internal register. It can be
 * also used to reset the device ROM from an unknown condition" After the soft
 * reset is performed, there is a delay before the device can be accessed.
 * Datasheet claims 2.8ms, emperically 10ms or greater is recommended.
 *
 * @param cfg I2C configuration for this device
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_soft_reset(const hal_i2c_config_t* cfg);

/** @brief Read PROM value from given address
 *
 * Reads a PROM value from the given address and stores it in passed in
 * variable. It is recommend to use the utility function
 * ms5525dso_read_all_coeff() to get all 8 PROM values. Note: While the
 * conversion formula only utilizes 6 PROM values, all 8 PROM values are
 * required to perform the CRC4 check which validates the PROM readings
 *
 * @param cfg I2C configuration for this device
 * @param prom_addr Address of PROM to read
 * @param prom_value Pointer to 16-bit unsigned integer that will be filled in
 * with read PROM value
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_read_prom(const hal_i2c_config_t* cfg, uint8_t prom_addr,
                              uint16_t* prom_value);

/** @brief Read ADC channel conversion result
 *
 * This will get the results of the last conversion of last requested channel.
 * Each result is returne as a 24-bit unsigned integer, but stored in a 32-bit
 * unsigned integer. It is important that this readback occur after the
 * requisite time has passed for the channel ADC to have finished conversion.
 * See the datasheet for timing details based on the over sample rate selected
 * at start of conversion.
 *
 * @param cfg I2C configuration for this device
 * @param adc_value Pointer to variable where adc channel value will be stored
 * after readback
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_read_adc(const hal_i2c_config_t* cfg, uint32_t* adc_value);

/** @brief Read all PROM values
 *
 * Reads all 8 PROM addresses and fills in the full coefficient table. This
 * function performs the CRC4 check for you, and will return ESP_FAIL if the
 * CRC4 is not a match to the calculated value
 *
 * @param cfg I2C configuration for this device
 * @param coeff Pointer to structure where the PROM values will be stored
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_read_all_coeff(const hal_i2c_config_t* cfg,
                                   ms5525dso_coeff_t* coeff);

/** @brief Start ADC channel conversion
 *
 * There are two ADC channels on the MS5525DSO, presssure and temperature. This
 * will allow you to select one or the other, and what over sample rate (OSR) to
 * use, and then start the conversion. It is important to note that the larger
 * the OSR, the longer the conversion will take, up to ~10ms.
 *
 * @param cfg I2C configuration for this device
 * @param ch Which ADC channel to start conversion on Pressure (D1) or
 * Temperature (D2)
 * @param osr What over sample rate to use when performing the conversion
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_start_ch_convert(const hal_i2c_config_t* cfg,
                                     ms5525dso_ch_e ch, ms5525dso_osr_e osr);

/** @brief Calculate the CRC4 of a coefficient table
 *
 * Each Coefficient table stores a CR4 value in the lower 4-bits of the final
 * PROM address, address 7. A calculation can be done to recreate that CRC4
 * value, that uses all 8 PROM values, with the calculated CRC4 zero'd out.
 *
 * @param coeff Coefficient table to calculate the CRC4
 * @return Calculated CRC4 value
 */
uint8_t ms5525dso_calculate_coeff_crc(const ms5525dso_coeff_t* coeff);

/** @brief Calculate the compensated pressure and temperature
 *
 * This functions calculates the compensated pressure and temperature using the
 * read out D1 (pressure), D2 (temperature), manufacturer provided QX table, and
 * the device provided calibrated coefficent table.
 *
 * @param osr Oversample rate to use
 * @param qx Pointer to QX table to use, determined by manufacturer model
 * @param coeff Pointer to coefficient table to use
 * @param d1 Pressure ADC channel value to use
 * @param d2 Temperature ADC channel value to use
 * @param p_compensated Pointer to float where calculated compensated presssure
 * will be stored
 * @param t_compensated Pointer to float where calculated compensated
 * temperature will be stored
 * @return HAL_OK if no error, hal_err_t value otherwise
 */
hal_err_t ms5525dso_calculate_pt(const ms5525dso_qx_t* qx,
                                 const ms5525dso_coeff_t* coeff, uint32_t d1,
                                 uint32_t d2, int32_t* p_compensated,
                                 int32_t* t_compensated);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // DRV_MS5525DSO_H
