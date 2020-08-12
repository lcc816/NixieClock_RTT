/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "i2c_adapter.h"
#include "multi_button.h"
#include "ds3231.h"
#include "buzzer.h"

/* Private define ------------------------------------------------------------*/
#define LED0_PIN    8  /* defined the LED0 pin: PA8 */
#define LED1_PIN    50 /* defined the LED1 pin: PD2 */
#define KEY0_PIN    4 /* PA4 */
#define KEY1_PIN    5 /* PA5 */

#define KEY0_PRESS_LEVEL 0
#define KEY1_PRESS_LEVEL 0

/* Private variables ---------------------------------------------------------*/
struct button key0, key1, key2; /* 实例化 3 个按键 */
static rt_timer_t btn_timer;
static rt_bool_t beep = RT_FALSE;
static int alarm_count = 0;

/* Private function prototypes -----------------------------------------------*/
static rt_uint8_t key0_pin_level(void)
{
    return rt_pin_read(KEY0_PIN);
}

static rt_uint8_t key1_pin_level(void)
{
    return rt_pin_read(KEY1_PIN);
}

static void button_timer_handler(void *param)
{
    button_ticks();
}

static void alarm_clock_handler(void *args)
{
    if (rt_pin_read(DS3231_SQW_PIN) == PIN_LOW)
    {
        rt_kprintf("alarm %d\n", alarm_count++);
    }
}

static void key0_single_clicked_handler(void *key);
static void key0_long_pressed_handler(void *key);
static void key1_single_clicked_handler(void *key);
static void key1_long_pressed_handler(void *key);

int main(void)
{
    /* set LED pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED0_PIN, PIN_HIGH);
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED1_PIN, PIN_HIGH);

    DS3231_Init();

    beep_init();

    /* 设置闹钟中断引脚: 上拉输入, 下降沿触发 */
    rt_pin_mode(DS3231_SQW_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(DS3231_SQW_PIN, PIN_IRQ_MODE_FALLING, alarm_clock_handler, RT_NULL);
    rt_pin_irq_enable(DS3231_SQW_PIN, PIN_IRQ_ENABLE);

    /* 按下时电平为 0 则上拉输入，反之则下拉输入 */
    rt_pin_mode(KEY0_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY1_PIN, PIN_MODE_INPUT_PULLUP);

    button_init(&key0, key0_pin_level, KEY0_PRESS_LEVEL);
    button_init(&key1, key1_pin_level, KEY1_PRESS_LEVEL);

    button_attach(&key0, SINGLE_CLICK,      key0_single_clicked_handler);
    button_attach(&key0, LONG_RRESS_START,  key0_long_pressed_handler);
    button_attach(&key1, SINGLE_CLICK,      key1_single_clicked_handler);
    button_attach(&key1, LONG_RRESS_START,  key1_long_pressed_handler);

    button_start(&key0);
    button_start(&key1);

    btn_timer = rt_timer_create("btn_timer", button_timer_handler, RT_NULL,
                                RT_TICK_PER_SECOND/200,
                                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    if (btn_timer != RT_NULL)
        rt_timer_start(btn_timer);

    while (1)
    {
        rt_pin_write(LED1_PIN, !rt_pin_read(LED1_PIN));
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}

/*******************************************************************************
  * @brief  key0 单击事件
*******************************************************************************/
void key0_single_clicked_handler(void *key)
{
    rt_kprintf("key0 is clicked.\n");
    beep = RT_TRUE;
}

/*******************************************************************************
  * @brief  key0 长按事件
*******************************************************************************/
void key0_long_pressed_handler(void *key)
{
    rt_kprintf("key0 is long pressed\n");
}

/*******************************************************************************
  * @brief  key1 单击事件
*******************************************************************************/
void key1_single_clicked_handler(void *key)
{
    rt_kprintf("key1 is clicked.\n");
}

/*******************************************************************************
  * @brief  key1 长按事件
*******************************************************************************/
void key1_long_pressed_handler(void *key)
{
    rt_kprintf("key1 is long pressed.\n");
}
