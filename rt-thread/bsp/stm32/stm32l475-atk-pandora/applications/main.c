/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-6-029     ye   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <led_alert.h>
#include <temp_humi_th.h>
#include <tsl4531_th.h>
#include <mylog.h>
#include <key.h>
int main(void)
{ int cnt=0;
  key_int();
	led_init();
	
//	mylog_init();
//	temp_humi_int();
//	lux_int();
  
  
 // led_flash_thread(); 
//	while(1)
//	{
//		cnt++;
//	  mylog("main",&cnt);
//		rt_thread_mdelay(1000);
//	}
	
}
