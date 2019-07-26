/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-25      YE           temp_humi
*/
#include <rtdevice.h>
#include <tsl4531_th.h>
#include <sensor.h>
#include <mylog.h>
#include <led_alert.h>
#include <key.h>
#define TSL4531_HUMI_NAME          "li_tsl4531"  /*  */

static rt_device_t sensor_lux_tsl4531; 
extern rt_uint8_t tsl4531_enflag;
void lux_int()
{ 
	rt_thread_t iotb_tsl4531_tid;
  sensor_lux_tsl4531 = rt_device_find(TSL4531_HUMI_NAME);
	rt_device_open(sensor_lux_tsl4531, RT_DEVICE_OFLAG_RDWR );

  iotb_tsl4531_tid = rt_thread_create("lux",
        lux_entry,
        RT_NULL,
        1024, 6, 10);
    if (iotb_tsl4531_tid != RT_NULL)
        rt_thread_startup(iotb_tsl4531_tid);

}

void lux_entry()
{
  struct rt_sensor_data tsl4531_data;
	rt_uint32_t lux_data;

 while(1)
 { 
	 if(tsl4531_enflag==LUX_ON)
	 {
     rt_device_read(sensor_lux_tsl4531, 0,&tsl4531_data, 1);
	   lux_data=tsl4531_data.data.light;
	   //if(lux_data>200) led_blink();
	   mylog("lux",&lux_data);
	   //rt_kprintf("lux =%d \n",lux_data);
	   
	 }
	 rt_thread_mdelay(1000);
 }
}

