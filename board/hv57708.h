/*******************************************************************************
* @file     --> hv57708.h
* @author   --> Li.Changchun
* @version  --> 1.0
* @date     --> 23-Nov-2019
* @brief    --> HV57708 驱动头文件
*               HV57708_CLK - CLK
*               HV57708_LE  - LE
*               HV57708_POL - POL
*               HV57708_DI1 - Din1/Dout4(A)
*               HV57708_DI2 - Din2/Dout3(A)
*               HV57708_DI3 - Din3/Dout2(A)
*               HV67708_DI4 - Din4/Dout1(A)
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HV57708_H
#define __HV57708_H

/* Includes ------------------------------------------------------------------*/
#include <drv_common.h>
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define HV57708_CLK         43
#define HV57708_LE          44
#define HV57708_POL         42
#define HV57708_DI1         31
#define HV57708_DI2         30
#define HV57708_DI3         29
#define HV57708_DI4         28
#define HV57708_SW          19

#define HV57708_CLK_H       rt_pin_write(HV57708_CLK, PIN_HIGH)
#define HV57708_LE_H        rt_pin_write(HV57708_LE, PIN_HIGH)
#define HV57708_POL_H       rt_pin_write(HV57708_POL, PIN_HIGH)
#define HV57708_DIN1_H      rt_pin_write(HV57708_DI1, PIN_HIGH)
#define HV57708_DIN2_H      rt_pin_write(HV57708_DI2, PIN_HIGH)
#define HV57708_DIN3_H      rt_pin_write(HV57708_DI3, PIN_HIGH)
#define HV57708_DIN4_H      rt_pin_write(HV57708_DI4, PIN_HIGH)

#define HV57708_CLK_L       rt_pin_write(HV57708_CLK, PIN_LOW)
#define HV57708_LE_L        rt_pin_write(HV57708_LE, PIN_LOW)
#define HV57708_POL_L       rt_pin_write(HV57708_POL, PIN_LOW)
#define HV57708_DIN1_L      rt_pin_write(HV57708_DI1, PIN_LOW)
#define HV57708_DIN2_L      rt_pin_write(HV57708_DI2, PIN_LOW)
#define HV57708_DIN3_L      rt_pin_write(HV57708_DI3, PIN_LOW)
#define HV57708_DIN4_L      rt_pin_write(HV57708_DI4, PIN_LOW)

/* Exported functions ------------------------------------------------------- */
void HV57708_Init(void);
void HV57708_TubePower(rt_base_t NewState);
rt_base_t HV57708_TubePowerStatus(void);
void HV57708_SendData(uint32_t datapart2, uint32_t datapart1);
void HV57708_OutputData(void);
void HV57708_Display(unsigned char data[]);
void HV57708_Protection(void);
/*测试用*/
void HV57708_Scan(void);
void HV57708_SetPin(uint8_t pin);

#endif /* __HV57708_H */
