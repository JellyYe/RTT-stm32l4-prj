/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           key
*/

#ifndef KEY_H
#define KEY_H


#include <rtdevice.h>
#include <board.h>
#define TEMP_HUMI_ON 1
#define TEMP_HUMI_OFF 0
#define LUX_ON 1
#define LUX_OFF 0

void key_cb(void *args);
void key_int();

#endif
