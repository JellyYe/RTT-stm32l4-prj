/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           key
*/
#include <key.h>
#include "drv_gpio.h"
#include <drv_log.h>
/* 中断回调函数 */
rt_uint8_t ath10_enflag=0;
rt_uint8_t tsl4531_enflag=0;  
rt_uint8_t pressflag=0;	
void key0_cb(void *args)
{  
 
	 static rt_uint8_t cnt;
    if(rt_pin_read(KEY0_PIN_NUM)==PIN_LOW) 
			{
				rt_pin_write(PIN_LED_G, PIN_LOW);
				rt_kprintf("key0 press\n");
			  pressflag=1;
			}
    else 
			{
				rt_pin_write(PIN_LED_G, PIN_HIGH);
				rt_kprintf("key0 release\n");
				if(pressflag==1)
					{
						cnt++;
						pressflag=0;
					}
				
				if(cnt%2)
					{
						ath10_enflag=TEMP_HUMI_ON;
						rt_kprintf("ath10 open\n");
					}
				else 
					{
						ath10_enflag=TEMP_HUMI_OFF;
						rt_kprintf("ath10 closs\n");
					}
				//LOG_I("ath10_enflag=%d",ath10_enflag);
			}
    
}
/* 中断回调函数 */
void key1_cb(void *args)
{  static rt_uint8_t cnt;
    if(rt_pin_read(KEY1_PIN_NUM)==PIN_LOW) 
			{
				rt_pin_write(PIN_LED_G, PIN_LOW);
				pressflag=1;
				rt_kprintf("key1 press\n");
			}
    else 
			{
				rt_pin_write(PIN_LED_G, PIN_HIGH);
				rt_kprintf("key1 release\n");
				if(pressflag==1)
					{
						cnt++;
						pressflag=0;
					}
				if(cnt%2)
					{
						tsl4531_enflag=LUX_ON;
						rt_kprintf("tsl4531 open\n");
					}
				else 
					{
						tsl4531_enflag=LUX_OFF;
						rt_kprintf("tsl4531 closs\n");
					}

			}
    
}

/* 中断回调函数 */
void key2_cb(void *args)
{
    if(rt_pin_read(KEY2_PIN_NUM)==PIN_LOW) 
			{
				rt_pin_write(PIN_LED_G, PIN_LOW);
				rt_kprintf("key3 press,red led open\n");
			}
    else 
			{
				rt_pin_write(PIN_LED_G, PIN_HIGH);
				rt_kprintf("key3 release,red led closs\n");
			}
    
}
void key_int()
{
    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLUP);
	  rt_pin_mode(KEY1_PIN_NUM, PIN_MODE_INPUT_PULLUP);
	  rt_pin_mode(KEY2_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，边沿触发 key_cb为回调函数*/
    rt_pin_attach_irq(KEY0_PIN_NUM, PIN_IRQ_MODE_RISING_FALLING , key0_cb, RT_NULL);
	  rt_pin_attach_irq(KEY1_PIN_NUM, PIN_IRQ_MODE_RISING_FALLING , key1_cb, RT_NULL);
	  rt_pin_attach_irq(KEY2_PIN_NUM, PIN_IRQ_MODE_RISING_FALLING , key2_cb, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY0_PIN_NUM, PIN_IRQ_ENABLE);
	  rt_pin_irq_enable(KEY1_PIN_NUM, PIN_IRQ_ENABLE);
	  rt_pin_irq_enable(KEY2_PIN_NUM, PIN_IRQ_ENABLE);

    //key
}

//INIT_BOARD_EXPORT(key_int);