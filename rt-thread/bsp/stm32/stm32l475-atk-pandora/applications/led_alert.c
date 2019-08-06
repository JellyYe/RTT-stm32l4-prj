/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           LED
*/
#include <rtthread.h>
#include <board.h>
#include <drivers/pin.h>
#include "drv_gpio.h"
void led_init() 
{
        /* set RGB_LED pin mode to output */
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_G, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);
    rt_pin_write(PIN_LED_R, PIN_HIGH);
    rt_pin_write(PIN_LED_G, PIN_HIGH);
    rt_pin_write(PIN_LED_B, PIN_HIGH);
} 
INIT_COMPONENT_EXPORT(led_init);

void led_on(rt_int8_t leddevice)
{
     rt_pin_write(leddevice, PIN_LOW);
}    
void led_off(rt_int8_t leddevice)
{
    rt_pin_write(leddevice, PIN_HIGH);
}
void led_blink()
{
    while (1)
    {
        led_on(PIN_LED_R);
        rt_thread_mdelay(500);
        led_off(PIN_LED_R);
        rt_thread_mdelay(500);
    }

    
}

//rt_err_t led_flash_thread(void)
//{
//    rt_thread_t iotb_led_flash_tid;

//    //led_init() ;
//    iotb_led_flash_tid = rt_thread_create("led_flash_thread",
//        led_blink,
//        RT_NULL,
//        512, 6, 10);
//    if (iotb_led_flash_tid != RT_NULL)
//        rt_thread_startup(iotb_led_flash_tid);
//    return RT_EOK;
//}
/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(i2c_aht10_sample, i2c aht10 sample);