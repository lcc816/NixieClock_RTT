/*******************************************************************************
* @file     i2c_adapter.c
* @author   lcc
* @version  1.0
* @date     7-Jun-2020
* @brief    将 RT_Thread 的 I2C 总线传输接口封装成易于操作传感器的接口
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_adapter.h"
#include <rtdevice.h>

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
rt_bool_t i2c_initialized = RT_FALSE;
static struct rt_i2c_bus_device *i2c_bus = RT_NULL; /* I2C总线设备句柄 */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* @brief    初始化 I2C 设备 (获取 I2C 句柄)
* @param    name - 已经注册过的 I2C 总线名称
* @retval   None
*******************************************************************************/
rt_err_t I2c_Init(const char *name)
{
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(name);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("[%d]%s(): can't find device %s!\n", __LINE__, __func__, name);
        return -RT_ERROR;
    }

    i2c_initialized = RT_TRUE;
    return RT_EOK;
}

/*******************************************************************************
* @brief    I2C 向指定从机发送一个字节
* @param    SlaveAddress  - 从机地址
* @param    txByte    - 被发送的字节
* @retval   RT_EOK: 成功, -RT_ERROR: 失败
*******************************************************************************/
rt_err_t I2c_WriteByte(uint8_t SlaveAddress, uint8_t txByte)
{
    struct rt_i2c_msg msgs;

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_WR;
    msgs.buf = &txByte;
    msgs.len = 1;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

/*******************************************************************************
* @brief    I2C 从指定的从机读取一个字节
* @param    SlaveAddress  - 从机地址
* @param    rxByte        - 指向读取数据的指针
* @retval   RT_EOK: 成功, -RT_ERROR: 失败
*******************************************************************************/
rt_err_t I2c_ReadByte(uint8_t SlaveAddress, uint8_t *rxByte)
{
    struct rt_i2c_msg msgs;

    if (rxByte == NULL)
    {
        return -RT_ERROR;
    }

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_RD;
    msgs.buf = rxByte;
    msgs.len = 1;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

/*******************************************************************************
* @brief    I2C 写一个字节数据到从机的指定地址
* @param    SlaveAddress  - 从机地址
* @param    REG_Address   - 寄存器地址
* @param    REG_data      - 字节数据
* @retval   RT_EOK/-RT_ERROR
*******************************************************************************/
rt_err_t I2c_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
    rt_uint8_t buf[2];
    struct rt_i2c_msg msgs;

    buf[0] = REG_Address;
    buf[1] = REG_data;

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 2;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

/*******************************************************************************
* @brief    I2C 从从机的指定地址读取一个字节
* @param    SlaveAddress  - 从机地址
* @param    REG_Address   - 寄存器地址
* @param    REG_data      - 指向读取字节的指针
* @retval   RT_EOK: 成功, -RT_ERROR: 失败
*******************************************************************************/
rt_err_t I2c_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data)
{
    /* 先写寄存器地址，再读取数据 */
    struct rt_i2c_msg msgs;

    if (REG_data == NULL)
    {
        return -RT_ERROR;
    }

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_WR;
    msgs.buf = &REG_Address;
    msgs.len = 1;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    msgs.flags = RT_I2C_RD;
    msgs.buf = REG_data;
    msgs.len = 1;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

/*******************************************************************************
* @brief    I2C 向从机的指定地址写入 n 个字节
* @param    SlaveAddress  - 从机地址
* @param    REG_Address   - 寄存器地址
* @param    len           - 字节数据
* @param    buf           - 发送数据缓冲区
* @retval   RT_EOK: 成功, -RT_ERROR: 失败
*******************************************************************************/
rt_err_t I2c_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    struct rt_i2c_msg msgs;
    uint8_t *pBuf;
    rt_err_t ret = -RT_ERROR;

    if (NULL == buf)
    {
        return -RT_ERROR;
    }

    pBuf = rt_malloc(len + 1);
    if (NULL == pBuf)
    {
        return -RT_ERROR;
    }

    pBuf[0] = REG_Address;
    rt_memcpy(&pBuf[1], buf, len);

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_WR;
    msgs.buf = pBuf;
    msgs.len = len + 1;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) == 1)
    {
        ret = RT_EOK;
    }

    rt_free(pBuf);
    return ret;
}

/*******************************************************************************
* @brief    I2C 从从机的指定地址读取 n 个字节
* @param    SlaveAddress  - 从机地址
* @param    REG_Address   - 寄存器地址
* @param    len           - 字节数据
* @param    buf           - 接收数据缓冲区
* @retval   RT_EOK: 成功, -RT_ERROR: 失败
*******************************************************************************/
rt_err_t I2c_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    if (buf == NULL)
    {
        return -RT_ERROR;
    }

    msgs.addr = SlaveAddress;
    msgs.flags = RT_I2C_WR;
    msgs.buf = &REG_Address;
    msgs.len = 1;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    if (rt_i2c_transfer(i2c_bus, &msgs, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
