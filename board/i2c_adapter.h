/*******************************************************************************
* @file     i2c_util.h
* @author   lcc
* @version  1.0
* @date     7-Jun-2020
* @brief    将 RT_Thread 的 I2C 总线传输接口封装成易于操作传感器的接口
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_UTIL_H
#define __I2C_UTIL_H

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

/* Exported variables --------------------------------------------------------*/
extern rt_bool_t i2c_initialized;

/* Exported type -------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/* 通用函数 */
rt_err_t I2c_Init(const char *name);
rt_err_t I2c_WriteByte(uint8_t SlaveAddress, uint8_t txByte);
rt_err_t I2c_ReadByte(uint8_t SlaveAddress, uint8_t *rxByte);

/* 读写从机寄存器 */
rt_err_t I2c_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
rt_err_t I2c_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data);
rt_err_t I2c_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
rt_err_t I2c_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);

#endif /* __I2C_UTIL_H */
