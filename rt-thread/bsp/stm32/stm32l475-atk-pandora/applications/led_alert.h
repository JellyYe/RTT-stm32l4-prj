/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           LED
*/

#ifndef LED_ALERT_H
#define LED_ALERT_H

#include <rtthread.h>


void led_init();   //接口可以初始化led
void led_on(rt_int8_t leddevice);
void led_off(rt_int8_t leddevice);
int led_blink();  //接口可以使led闪烁，200ms闪烁周期
rt_err_t led_flash_thread(void);


#endif
