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

#include <assert.h>
#include <drv_i2c_ms5525dso.h>
#include <hal_interface.h>
#include <stdint.h>

hal_err_t ms5525dso_soft_reset(const hal_i2c_config_t* cfg) {
  uint8_t cmd;

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  // Be aware, after a reset, we must wait 10 msec before usage
  cmd = MS5525DSO_REG_RESET;
  return hal_i2c_write(cfg, &cmd, sizeof(cmd));
}

hal_err_t ms5525dso_read_prom(const hal_i2c_config_t* cfg, uint8_t prom_addr,
                              uint16_t* prom_value) {
  hal_err_t res;
  uint8_t cmd;
  uint8_t prom_data[MS5525DSO_NUM_PROM_BYTES];

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  assert(prom_value);
  if (!prom_value) {
    return HAL_ERR_FAIL;
  }

  // If readback fails, prom_value will be zero
  *prom_value = 0;

  // Send command to select PROM address to read
  cmd =
      MS5525DSO_REG_PROM_READ_BASE | ((prom_addr & MS5525DSO_REG_PROM_ADDR_MASK)
                                      << MS5525DSO_REG_PROM_ADDR_OFST);
  res = hal_i2c_write(cfg, &cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  // Read selected PROM address
  res = hal_i2c_read(cfg, prom_data, MS5525DSO_NUM_PROM_BYTES);
  if (res != HAL_OK) {
    return res;
  }

  // Update passed in prom_value, MSB is in the first byte
  *prom_value = (prom_data[0] << 8) | prom_data[1];

  return res;
}

hal_err_t ms5525dso_read_adc(const hal_i2c_config_t* cfg, uint32_t* adc_value) {
  hal_err_t res;
  uint8_t cmd;
  uint8_t adc_data[MS5525DSO_NUM_ADC_BYTES];

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  assert(adc_value);
  if (!adc_value) {
    return HAL_ERR_FAIL;
  }

  *adc_value = 0;

  cmd = MS5525DSO_REG_ADC_READ;
  res = hal_i2c_write(cfg, &cmd, sizeof(cmd));
  if (res != HAL_OK) {
    return res;
  }

  res = hal_i2c_read(cfg, adc_data, MS5525DSO_NUM_ADC_BYTES);
  if (res != HAL_OK) {
    return res;
  }

  // Shift in data, MSB comes first
  *adc_value = (adc_data[0] << 16) | (adc_data[1] << 8) | adc_data[2];

  return res;
}

hal_err_t ms5525dso_start_ch_convert(const hal_i2c_config_t* cfg,
                                     ms5525dso_ch_e ch, ms5525dso_osr_e osr) {
  uint8_t cmd;

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  // Get Channel to convert
  switch (ch) {
    case MS5525DSO_CH_D1_PRESSURE:
      cmd = 0x40;
      break;
    case MS5525DSO_CH_D2_TEMPERATURE:
      cmd = 0x50;
      break;
    default:
      return HAL_ERR_FAIL;
  }  // switch(ch)

  // Get Over-sample Rate to use
  switch (osr) {
    case MS5525DSO_OSR256:
      cmd |= 0x00;
      break;
    case MS5525DSO_OSR512:
      cmd |= 0x02;
      break;
    case MS5525DSO_OSR1024:
      cmd |= 0x04;  // min/typ/max: 1.88 2.08 2.28 ms
      break;
    case MS5525DSO_OSR2048:
      cmd |= 0x06;  // min/typ/max: 3.72 4.13 4.54 ms
      break;
    case MS5525DSO_OSR4096:
      cmd |= 0x08;  // min/typ/max: 7.40 8.22 9.04 ms
      break;
    default:
      return HAL_ERR_FAIL;
  }  // switch(osr)

  // Initiate conversion of selected channel, at chosen over-sample rate
  return hal_i2c_write(cfg, &cmd, sizeof(cmd));
}

hal_err_t ms5525dso_read_all_coeff(const hal_i2c_config_t* cfg,
                                   ms5525dso_coeff_t* coeff) {
  uint8_t n;
  uint16_t value;
  hal_err_t res;

  assert(cfg);
  if (!cfg) {
    return HAL_ERR_FAIL;
  }

  assert(coeff);
  if (!coeff) {
    return HAL_ERR_FAIL;
  }

  res = HAL_ERR_FAIL;

  // Loop through the addresses and gather up the PROM values + the 4-bit CRC
  for (n = 0; n < MS5525DSO_NUM_PROM_ADDR; n++) {
    res = ms5525dso_read_prom(cfg, n, &value);
    if (res != HAL_OK) {
      // Break out of the loop and return the error
      break;
    }
    coeff->c[n] = value;
  }

  // Check the CRC4 only if the read appears to have worked
  if (res == HAL_OK) {
    if (ms5525dso_calculate_coeff_crc(coeff) != (coeff->c[7] & 0x000F)) {
      return HAL_ERR_FAIL;
    }
  }

  return res;
}

uint8_t ms5525dso_calculate_coeff_crc(const ms5525dso_coeff_t* coeff) {
  uint8_t cnt;
  uint8_t n_bit;
  uint16_t n_rem;
  uint16_t n_prom[MS5525DSO_NUM_PROM_ADDR];

  assert(coeff);
  if (!coeff) {
    return 0;
  }

  // Copy coefficient table into a local one, as we have to modified it
  n_prom[0] = coeff->c[0];
  n_prom[1] = coeff->c[1];
  n_prom[2] = coeff->c[2];
  n_prom[3] = coeff->c[3];
  n_prom[4] = coeff->c[4];
  n_prom[5] = coeff->c[5];
  n_prom[6] = coeff->c[6];
  n_prom[7] = (0xFF00 & (coeff->c[7]));  // CRC4 is calculated with the low byte
                                         // of address 7 zero'd

  n_rem = 0x00;

  // Go through low and high bytes of each 16-bit PROM address
  for (cnt = 0; cnt < (MS5525DSO_NUM_PROM_ADDR * 2); cnt++) {
    // choose LSB or MSB
    n_rem ^= (cnt % 2 == 1) ? (uint16_t)((n_prom[cnt >> 1]) & 0x00FF)
                            : (uint16_t)(n_prom[cnt >> 1] >> 8);
    for (n_bit = 8; n_bit > 0; n_bit--) {
      n_rem = (n_rem & (0x8000)) ? (n_rem << 1) ^ 0x3000 : (n_rem << 1);
    }
  }

  return (0x000F & (n_rem >> 12));  // // final 4-bit reminder is CRC code
}

hal_err_t ms5525dso_calculate_pt(const ms5525dso_qx_t* qx,
                                 const ms5525dso_coeff_t* coeff, uint32_t d1,
                                 uint32_t d2, int32_t* p_compensated,
                                 int32_t* t_compensated) {
  int64_t dT;
  int32_t TEMP;
  int64_t OFF;
  int64_t SENS;

  assert(coeff);
  assert(p_compensated);
  assert(t_compensated);
  if ((!coeff) || (!p_compensated) || (!t_compensated)) {
    return HAL_ERR_FAIL;
  }

  *p_compensated = 0;
  *t_compensated = 0;

  // Difference between actual and reference temperature
  // dT = D2 - TRE F = D2 - C5 * 2^Q5
  dT = d2 - (uint32_t)coeff->c[5] * (1 << qx->Q5);

  // Measured temperature
  // TEMP=20°C+dT*TEMPSENS=2000+dT*C6/2^Q6
  TEMP = 2000 + dT * (uint32_t)coeff->c[6] / (1 << qx->Q6);

  // Offset at actual temperature
  // OFF=OFFT1+TCO * dT
  // = C2*2^Q2+(C4*dT)/2^Q4
  OFF = (int64_t)coeff->c[2] * (1 << qx->Q2) +
        ((int64_t)coeff->c[4] * dT) / (1 << qx->Q4);

  // Sensitivity at actual temperature
  // SENS=SENST1+TCS*dT=C1*2^Q1+(C3*dT)/2^Q3
  SENS = (int64_t)coeff->c[1] * (1 << qx->Q1) +
         ((int64_t)coeff->c[3] * dT) / (1 << qx->Q3);

  // Temperature Compensated Pressure
  // P=D1*SENS-OFF=(D1*SENS/2^21-OFF)/2^15
  *p_compensated = (d1 * SENS / (1 << 21) - OFF) / (1 << 15);

  *t_compensated = TEMP;

  return HAL_OK;
}
