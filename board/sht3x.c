/*******************************************************************************
* @file     sht3x.c
* @author   lcc
* @version  1.0
* @date     22-July-2020
* @brief    SHT3x 温湿度传感器驱动接口
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sht3x.h"
#include <rtdevice.h>

/* Private define ------------------------------------------------------------*/

/* Generator polynomial for CRC */
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

/* Private variables ---------------------------------------------------------*/

/* Static functions ----------------------------------------------------------*/
// 主要对 soft I2C 的函数进行封装

/* write a command to sht3x */
static rt_err_t write_cmd(sht3x_device_t dev, rt_uint16_t cmd)
{
    rt_uint8_t buf[2] = {0};
    struct rt_i2c_msg msg;

    buf[0] = cmd >> 8;
    buf[1] = cmd & 0xFF;

    msg.addr = dev->i2c_addr;
    msg.flags = RT_I2C_WR;
    msg.buf = buf;
    msg.len = sizeof(buf);

    if (rt_i2c_transfer(dev->i2c, &msg, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/* calculate CRC value of bytes in buffer */
static rt_uint8_t cal_crc(void *data, rt_uint8_t len)
{
    rt_uint8_t bit;
    rt_uint8_t crc = 0xFF;
    rt_uint8_t cnt;
    rt_uint8_t *pt = (rt_uint8_t *)data;

    /* calculates 8-bit checksum with given polynomial */
    for (cnt = 0; cnt < len; cnt++)
    {
        crc ^= pt[cnt];
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else            crc = (crc << 1);
        }
    }

    return crc;
}

static rt_err_t read_bytes(sht3x_device_t dev, rt_uint8_t *data, rt_uint16_t len)
{
    struct rt_i2c_msg msg;

    msg.addr = dev->i2c_addr;
    msg.flags = RT_I2C_RD;
    msg.buf = data;
    msg.len = len;

    if (rt_i2c_transfer(dev->i2c, &msg, 1) != 1)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t read_two_bytes_and_crc(sht3x_device_t dev, rt_uint16_t *data)
{
    rt_uint8_t temp[3] = {0};

    RT_ASSERT(data);

    if (read_bytes(dev, temp, 3) == RT_EOK)
    {
        if (cal_crc(&temp[0], 2) == temp[2])
        {
            *data = temp[1] | (temp[0] << 8);
            return RT_EOK;
        }
    }

    return -RT_ERROR;
}

/*******************************************************************************
* @brief    创建一个 sht3x device 并初始化
* @param    i2c_bus_name - SHT3x 使用的 I2C 总线名称
* @param    sht3x_addr - SHT3x 在 I2C 总线上的地址
* @param    reset_pin - 硬件复位引脚（未使能硬件复位宏则无意义）
* @param    mode - 时钟模式，Stretch or Polling
* @param    rept - 重复测量精度
* @retval   创建成功返回 sh3x 的设备句柄, 否则返回 RT_NULL
*******************************************************************************/
sht3x_device_t sht3x_device_create(const char *i2c_bus_name, rt_uint8_t sht3x_addr,
                                       rt_base_t reset_pin,
                                       sht3x_clock_mode mode, sht3x_repeatability rept)

{
    sht3x_device_t dev;
    rt_uint32_t ser_num;

    RT_ASSERT(i2c_bus_name);

    dev = rt_calloc(1, sizeof(struct sht3x_device));
    if (dev == RT_NULL)
    {
        rt_kprintf("[%d]%s(): can't allocate memory for sht3x device %s\n", __LINE__, __func__, i2c_bus_name);
        return RT_NULL;
    }

    dev->i2c = (struct rt_i2c_bus_device *)rt_device_find(i2c_bus_name);
    if (dev->i2c == RT_NULL)
    {
        rt_kprintf("[%d]%s(): can't find %s\n", __LINE__, __func__, i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    if(sht3x_addr == SHT3X_ADDR_PD || sht3x_addr == SHT3X_ADDR_PU)
    {
        dev->i2c_addr = sht3x_addr;
    }
    else
    {
        rt_kprintf("[%d]%s(): illegal sht3x address: 0x%x\n", __LINE__, __func__, sht3x_addr);
        rt_free(dev);
        return RT_NULL;
    }

    dev->lock = rt_mutex_create("mutex_sht3x", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        rt_kprintf("[%d]%s(): can't create mutex for sht3x device\n", __LINE__, __func__);
        rt_free(dev);
        return RT_NULL;
    }

    dev->clock = mode;
    dev->rept = rept;

#ifdef SHT3X_ENABLE_RESET_PIN
    dev->reset_pin = reset_pin;
    rt_pin_mode(reset_pin, PIN_MODE_OUTPUT);
    rt_pin_write(reset_pin, PIN_HIGH);
#endif

    sht3x_clear_status(dev);

    return dev;
}

/*******************************************************************************
* @brief    SHT3x 反初始化
* @param    sht3x 设备句柄
* @retval   None
*******************************************************************************/
void sht3x_device_destroy(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);
    rt_free(dev);
}

/*******************************************************************************
* @brief    从 SHT3x 读取序列号
* @brief    dev - 指向 sht3x 设备指针
* @param    sn - 指向 32 位序列号的指针
* @retval   读取成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_read_serial_number(sht3x_device_t dev, rt_uint32_t *sn)
{
    rt_err_t ret;
    rt_uint8_t temp[6] = {0};

    RT_ASSERT(dev);
    RT_ASSERT(sn);

    if (rt_mutex_take(dev->lock, RT_WAITING_FOREVER) != RT_EOK)
    {
        rt_kprintf("[%d]%s(): can't take mutex of sht3x\n", __LINE__, __func__);
        return -RT_ERROR;
    }

    if (write_cmd(dev, SHT3X_CMD_READ_SERIALNUM) == RT_EOK)
    {
        rt_thread_mdelay(1);
        if (read_bytes(dev, temp, 6) == RT_EOK)
        {
            if ((cal_crc(&temp[0], 2) == temp[2]) && (cal_crc(&temp[3], 2) == temp[5]))
            {
                *sn = (temp[4] | (temp[3] << 8) | (temp[1] << 16) | (temp[0] << 24));
                ret = RT_EOK;
            }
        }
    }

    rt_mutex_release(dev->lock);
    return ret;
}

/*******************************************************************************
* @brief    读取温度值 [°C] 和相对湿度 [%RH]
* @param    dev - 指向 sht3x 设备指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_read_singleshot(sht3x_device_t dev)
{
    rt_uint16_t cmd;
    rt_err_t ret;
    rt_uint32_t ms;
    rt_uint8_t temp[6];

    rt_uint16_t commands[2][3] = {
        {SHT3X_CMD_MEAS_CLOCKSTR_H, SHT3X_CMD_MEAS_CLOCKSTR_M, SHT3X_CMD_MEAS_CLOCKSTR_L},
        {SHT3X_CMD_MEAS_POLLING_H, SHT3X_CMD_MEAS_POLLING_M, SHT3X_CMD_MEAS_POLLING_L}
    };

    RT_ASSERT(dev);

    cmd = commands[dev->clock][dev->rept];
    ms = (dev->clock == SHT3X_CLOCK_STRETCH) ? 1 : 5;

    ret = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (ret == RT_EOK)
    {
        if (write_cmd(dev, cmd) == RT_EOK)
        {
            rt_thread_mdelay(ms);
            if (read_bytes(dev, temp, 6) == RT_EOK)
            {
                if (cal_crc(&temp[0], 2) == temp[2])
                {
                    dev->temperature = -45.0 + (temp[1] | temp[0] << 8) * 175.0 / (0xFFFF - 1);
                    ret = RT_EOK;
                }
                if (cal_crc(&temp[3], 2) == temp[5])
                {
                    dev->humidity = (temp[4] | temp[3] << 8) * 0.0015259022;
                    ret = RT_EOK;
                }
            }
        }
        else
        {
            ret = -RT_ERROR;
            rt_kprintf("[%d]%s(): write cmd failed.\n", __LINE__, __func__);
        }
    }
    else
    {
        ret = -RT_ERROR;
        rt_kprintf("[%d]%s(): taking mutex of sht3x failed.\n", __LINE__, __func__);
    }

    return ret;
}

/*******************************************************************************
* @brief    调用软复位机制强制传感器进入确定状态, 不用移除电源
* @param    dev - 指向 sht3x 设备指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_soft_reset(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    return write_cmd(dev, SHT3X_CMD_SOFT_RESET);
}

#ifdef SHT3X_ENABLE_RESET_PIN

/*******************************************************************************
* @brief    硬件复位
* @param    None
* @retval   None
*******************************************************************************/
void sht3x_hard_reset(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    rt_pin_write(dev->reset_pin, PIN_LOW);
    rt_thread_mdelay(100);
    rt_pin_write(dev->reset_pin, PIN_LOW);
    rt_thread_mdelay(50);
}

#endif /* SHT3X_ENABLE_RESET_PIN */

/*******************************************************************************
* @brief    清除 SHT3x 状态寄存器的所有警告标志
* @param    dev - 指向 SHT3x 设备的指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_clear_status(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    return write_cmd(dev, SHT3X_CMD_CLEAR_STATUS);
}

/*******************************************************************************
* @brief    从 SHT3x 读取状态寄存器
* @param    dev - 指向 sht3x 设备指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_read_status(sht3x_device_t dev)
{
    rt_uint16_t data;

    RT_ASSERT(dev);

    if (read_two_bytes_and_crc(dev, &data) == RT_EOK)
    {
        dev->status.status_word = data;
        return RT_EOK;
    }

    return -RT_ERROR ;
}

/*******************************************************************************
* @brief    使能 heater
* @param    dev - 指向 sht3x 设备指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_enable_heater(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    if(write_cmd(dev, SHT3X_CMD_HEATER_ENABLE) == RT_EOK)
    {
        return RT_EOK ;
    }
    else
    {
        return -RT_ERROR ;
    }
}

/*******************************************************************************
* @brief    关闭 heater
* @param    dev - 指向 sht3x 设备指针
* @retval   操作成功返回 RT_EOK
*******************************************************************************/
rt_err_t sht3x_disable_heater(sht3x_device_t dev)
{
    RT_ASSERT(dev);

    if( write_cmd(dev, SHT3X_CMD_HEATER_DISABLE) == RT_EOK)
    {
        return RT_EOK ;
    }
    else
    {
        return -RT_ERROR ;
    }
}

