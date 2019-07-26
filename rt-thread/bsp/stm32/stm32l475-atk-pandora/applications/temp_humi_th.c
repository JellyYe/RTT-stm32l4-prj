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
#include <temp_humi_th.h>
#include <sensor.h>
#include <rtdbg.h>
#include <mylog.h>
#include <led_alert.h>
#include <key.h>
#define AHT10_HUMI_NAME          "humi_aht10"  /*  */
#define AHT10_TEMP_NAME          "temp_aht10"  /*  */
#define CHAR_NUMBER         10
static rt_device_t sensor_humi_aht10,sensor_temp_aht10; 
extern rt_uint8_t ath10_enflag;

static const char *TAG = "tmep_humi_th";

void temp_humi_int()
{ 
	rt_thread_t iotb_temp_humi_tid;
  sensor_humi_aht10 = rt_device_find(AHT10_HUMI_NAME);
	rt_device_open(sensor_humi_aht10, RT_DEVICE_OFLAG_RDWR );
	sensor_temp_aht10 = rt_device_find(AHT10_TEMP_NAME);
  rt_device_open(sensor_temp_aht10, RT_DEVICE_OFLAG_RDWR );

  iotb_temp_humi_tid = rt_thread_create("temp",
        temp_humi_entry,
        RT_NULL,
        1024, 6, 10);
    if (iotb_temp_humi_tid != RT_NULL)
        rt_thread_startup(iotb_temp_humi_tid);

}

void temp_humi_entry()
{
  struct rt_sensor_data aht10_data;
  rt_uint8_t buf[10];
	rt_uint32_t humi_data,temp_data;

 while(1)
 { 
	  
	 if(ath10_enflag==TEMP_HUMI_ON)
	 {
     rt_device_read(sensor_humi_aht10, 0,&aht10_data, 1);
	   humi_data=aht10_data.data.humi;
	 
			//LOG_W("%s:%d", TAG, humi_data);
			mylog("humi",&humi_data);
			rt_device_read(sensor_temp_aht10, 0,&aht10_data, 1);
	
			temp_data=aht10_data.data.temp;
	    if(temp_data/10>30) led_blink();
			mylog("temp",&temp_data);
			//rt_kprintf("humi=%d.%d   temp=%d.%d\n",humi_data/10,humi_data%10,temp_data/10,temp_data%10);
	    //LOG_I("aht10 biging");
	 }
	 rt_thread_mdelay(1000);
	 
 }
}

