/*******************************************************************************
* @file     buzzer.c
* @author   Lichangchun
* @version  v1.0
* @date     16-July-2019
* @brief    蜂鸣器接在 PC8, 使用 TIM8_CH3 PWM 输出
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <rtdevice.h> 
#include "buzzer.h"
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static struct rt_device_pwm *pwm_device = RT_NULL; // 定义 pwm 设备指针

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* @brief    蜂鸣器初始化
* @param    None
* @retval   -RT_ERROR 失败, RT_EOK 成功
*******************************************************************************/
int beep_init(void)
{
    pwm_device = (struct rt_device_pwm *)rt_device_find(BEEP_PWM_DEVICE);

    if (pwm_device == RT_NULL)
    {
        rt_kprintf("[%d]%s(): can't find device %s!\n", __LINE__, __func__, BEEP_PWM_DEVICE);
        return -RT_ERROR;
    }

    return RT_EOK;
}

/*******************************************************************************
* @brief    蜂鸣器开
* @param    None
* @retval   -RT_ERROR 失败, RT_EOK 成功
*******************************************************************************/
int beep_on(void)
{
    rt_pwm_enable(pwm_device, BEEP_PWM_CH); //使能蜂鸣器对应的 PWM 通道
    return 0;
}

/*******************************************************************************
* @brief    蜂鸣器开
* @param    None
* @retval   -RT_ERROR 失败, RT_EOK 成功
*******************************************************************************/
int beep_off(void)
{
    rt_pwm_disable(pwm_device, BEEP_PWM_CH); //失能蜂鸣器对应的 PWM 通道
    return 0;
}

/*******************************************************************************
* @brief    蜂鸣器设定频率和音量
* @param    freq 频率(Hz)
* @param    volume 音量
* @retval   -RT_ERROR 失败, RT_EOK 成功
*******************************************************************************/
int beep_set(uint16_t freq, uint8_t volume)
{
    rt_uint32_t period, pulse;

    /* 将频率转化为周期 周期单位:ns 频率单位:HZ */
    period = 1000000000 / freq;  //unit:ns 1/HZ*10^9 = ns

    /* 根据声音大小计算占空比 蜂鸣器低电平触发 */
    pulse = period - period / 100 * volume;

    /* 利用 PWM API 设定 周期和占空比 */
    rt_pwm_set(pwm_device, BEEP_PWM_CH, period, pulse);//channel,period,pulse

    return 0;
}

/*******************************************************************************
* @brief    Sound 1, 嘀~
* @param    None
* @retval   None
*******************************************************************************/
void beep_sound1(void)
{
    beep_set(2500, 50);
    beep_on();
    rt_thread_mdelay(500);
    beep_off();
}

/*******************************************************************************
* @brief    Sound 1, 嘀~嘀~嘀~
* @param    None
* @retval   None
*******************************************************************************/
void beep_sound2(void)
{
    beep_set(2500, 50);
    beep_on();
    rt_thread_mdelay(150);
    beep_off();
    rt_thread_mdelay(100);
    beep_on();
    rt_thread_mdelay(150);
    beep_off();
    rt_thread_mdelay(100);
    beep_on();
    rt_thread_mdelay(150);
    beep_off();
}
