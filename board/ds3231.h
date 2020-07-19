/*******************************************************************************
* @file    ds3231.h
* @author  lcc
* @version 1.0
* @date    5-Jun-2020
* @brief   DS3231 高精度实时时钟驱动头文件, I2C
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DS3231_H
#define __DS3231_H

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DS3231_Time;

typedef struct 
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DS3231_Clock;

typedef struct 
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t day; // 星期和日期是联动的
} DS3231_Date;

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#define  DS3231_SQW_PIN                 GET_PIN(C, 9)

/* 闹钟屏蔽位 */
#define  DS3231_A1_OncePerSecond          0x0F // 每秒
#define  DS3231_A1_SecondMatch            0x0E // 秒
#define  DS3231_A1_MinuteSecond           0x0C // 分-秒
#define  DS3231_A1_HourMinuteSecond       0x08 // 时-分-秒
#define  DS3231_A1_DateHourMinuteSecond   0x00 // 日期-时-分-秒
#define  DS3231_A1_DayHourMinuteSecond    0x10 // 星期-时-分-秒
#define  DS3231_A2_OncePerMinute        0x07 // 每分
#define  DS3231_A2_MinuteMatch          0x06 // 分
#define  DS3231_A2_HourMinute           0x04 // 时-分
#define  DS3231_A2_DateHourMinute       0x00 // 日期-时-分
#define  DS3231_A2_DayHourMinute        0x08 // 星期-时-分

/* Exported functions ------------------------------------------------------- */
void DS3231_Init(void);
void DS3231_GetTime(DS3231_Time *time);
void DS3231_GetClock(DS3231_Clock *clock);
void DS3231_GetDate(DS3231_Date *date);

void DS3231_SetTime(DS3231_Time *time);
void DS3231_SetClock(DS3231_Clock *clock);
void DS3231_SetDate(DS3231_Date *date);

void DS3231_SetAlarm1(uint8_t mode, DS3231_Time *time);
void DS3231_SetAlarm2(uint8_t mode, DS3231_Time *time);

void DS3231_EnableAlarmIT(uint8_t alarm);
void DS3231_DisableAlarmIT(uint8_t alarm);

rt_bool_t DS3231_CheckAlarmITEnabled(uint8_t alarm);
rt_bool_t DS3231_CheckIfAlarm(uint8_t alarm);

//float DS3231_GetTemperature(void);

#endif /* __DS3231_H */
