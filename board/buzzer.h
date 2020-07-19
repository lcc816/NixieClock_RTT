/*******************************************************************************
* @file     buzzer.h
* @author   lcc
* @version  
* @date     16-July-2020
* @brief    
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZZER_H
#define __BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#define BEEP_PWM_DEVICE     "pwm3"
#define BEEP_PWM_CH         3
#define BEEP_PWM_PIN        40

/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
int beep_init(void);                         //蜂鸣器初始化
int beep_on(void);                           //蜂鸣器开
int beep_off(void);                          //蜂鸣器关
int beep_set(uint16_t freq, uint8_t volume); //蜂鸣器设定

#endif /* __BUZZER_H */
