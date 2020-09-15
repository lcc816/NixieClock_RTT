/*******************************************************************************
* @file     sht3x.h
* @author   lcc
* @version  1.0
* @date     22-July-2020
* @brief    SHT3x 温湿度传感器头文件
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SHT3X_H
#define __SHT3X_H

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

/* Exported define -----------------------------------------------------------*/
/* SHT3X 的 I2C 地址 */
#define SHT3X_ADDR_PD 0x44 // addr pin pulled down: 0x44
#define SHT3X_ADDR_PU 0x45 // addr pin pulled down: 0x45

/* 根据硬件调整 Alert 和 Reset 引脚的宏开关和宏定义 */

// #define SHT3X_ENABLE_ALERT_PIN
// #define SHT3X_ENABLE_RESET_PIN

/* Exported type -------------------------------------------------------------*/
/* 传感器支持的指令 */
#define SHT3X_CMD_READ_SERIALNUM      0x3780 /* read serial number */
#define SHT3X_CMD_READ_STATUS         0xF32D /* read status register */
#define SHT3X_CMD_CLEAR_STATUS        0x3041 /* clear status register */
#define SHT3X_CMD_HEATER_ENABLE       0x306D /* enabled heater */
#define SHT3X_CMD_HEATER_DISABLE      0x3066 /* disable heater */
#define SHT3X_CMD_SOFT_RESET          0x30A2 /* sofloat *reset */
#define SHT3X_CMD_MEAS_CLOCKSTR_H     0x2C06 /* measurement: clock stretching, high repeatability */
#define SHT3X_CMD_MEAS_CLOCKSTR_M     0x2C0D /* measurement: clock stretching, medium repeatability */
#define SHT3X_CMD_MEAS_CLOCKSTR_L     0x2C10 /* measurement: clock stretching, low repeatability */
#define SHT3X_CMD_MEAS_POLLING_H      0x2400 /* measurement: polling, high repeatability */
#define SHT3X_CMD_MEAS_POLLING_M      0x240B /* measurement: polling, medium repeatability */
#define SHT3X_CMD_MEAS_POLLING_L      0x2416 /* measurement: polling, low repeatability */
#define SHT3X_CMD_MEAS_PERI_05_H      0x2032 /* measurement: periodic 0.5 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_05_M      0x2024 /* measurement: periodic 0.5 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_05_L      0x202F /* measurement: periodic 0.5 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_1_H       0x2130 /* measurement: periodic 1 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_1_M       0x2126 /* measurement: periodic 1 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_1_L       0x212D /* measurement: periodic 1 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_2_H       0x2236 /* measurement: periodic 2 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_2_M       0x2220 /* measurement: periodic 2 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_2_L       0x222B /* measurement: periodic 2 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_4_H       0x2334 /* measurement: periodic 4 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_4_M       0x2322 /* measurement: periodic 4 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_4_L       0x2329 /* measurement: periodic 4 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_10_H      0x2737 /* measurement: periodic 10 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_10_M      0x2721 /* measurement: periodic 10 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_10_L      0x272A /* measurement: periodic 10 mps, low repeatability */
#define SHT3X_CMD_MEAS_ART            0x2B32 /* measurement: periodic with accelerated response time */
#define SHT3X_CMD_FETCH_DATA          0xE000 /* readout measurements for periodic mode */
#define SHT3X_CMD_R_AL_LIM_LS         0xE102 /* read alert limits, low set */
#define SHT3X_CMD_R_AL_LIM_LC         0xE109 /* read alert limits, low clear */
#define SHT3X_CMD_R_AL_LIM_HS         0xE11F /* read alert limits, high set */
#define SHT3X_CMD_R_AL_LIM_HC         0xE114 /* read alert limits, high clear */
#define SHT3X_CMD_W_AL_LIM_HS         0x611D /* write alert limits, high set */
#define SHT3X_CMD_W_AL_LIM_HC         0x6116 /* write alert limits, high clear */
#define SHT3X_CMD_W_AL_LIM_LC         0x610B /* write alert limits, low clear */
#define SHT3X_CMD_W_AL_LIM_LS         0x6100 /* write alert limits, low set */
#define SHT3X_CMD_NO_SLEEP            0x303E

/* 重复测量精度 */
typedef enum
{
    SHT3X_REPEATAB_HIGH,   /* high repeatability */
    SHT3X_REPEATAB_MEDIUM, /* medium repeatability */
    SHT3X_REPEATAB_LOW,    /* low repeatability */
} sht3x_repeatability;

/* 测量模式 */
typedef enum
{
    SHT3X_CLOCK_STRETCH, /* clock stretching */
    SHT3X_CLOCK_POLLING, /* polling */
} sht3x_clock_mode;

typedef union sht3x_status_word
{
    rt_uint16_t status_word;
    struct status_bits
    {
        rt_uint16_t checksum_ok : 1 ;
        rt_uint16_t command_ok : 1 ;
        rt_uint16_t reserved_3 : 2 ;
        rt_uint16_t reset_detected : 1 ;
        rt_uint16_t reserved_2 : 5 ;
        rt_uint16_t T_tracking_alert : 1 ;
        rt_uint16_t RH_tracking_alert : 1 ;
        rt_uint16_t reserved_1 : 1 ;
        rt_uint16_t heater : 1 ;
        rt_uint16_t reserved_0 : 1 ;
        rt_uint16_t alert_pending: 1 ;
    } bits;
} sht3x_status;

struct sht3x_device
{
    struct rt_i2c_bus_device *i2c;
    rt_uint8_t i2c_addr;
    rt_mutex_t lock;

    rt_base_t reset_pin;

    sht3x_clock_mode clock;
    sht3x_repeatability rept;

    sht3x_status status;
    float temperature;
    float humidity;
};

typedef struct sht3x_device *sht3x_device_t;

/* Exported variables---------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
sht3x_device_t sht3x_device_create(const char *i2c_bus_name, rt_uint8_t sht3x_addr,
                                   rt_base_t reset_pin,
                                   sht3x_clock_mode mode, sht3x_repeatability rept);

void sht3x_device_destroy(sht3x_device_t dev);

rt_err_t sht3x_read_serial_number(sht3x_device_t dev, rt_uint32_t * sn);

rt_err_t sht3x_read_status(sht3x_device_t dev);

rt_err_t sht3x_clear_status(sht3x_device_t dev);

rt_err_t sht3x_read_singleshot(sht3x_device_t dev);

rt_err_t sht3x_soft_reset(sht3x_device_t dev);

#ifdef SHT3X_ENABLE_RESET_PIN
void sht3x_hard_reset(sht3x_device_t dev);
#endif

rt_err_t sht3x_enable_heater(sht3x_device_t dev);

rt_err_t sht3x_disable_heater(sht3x_device_t dev);

#endif /* __SHT3X_H */
