/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           key
*/

#ifndef __MYLOG_H
#define __MYLOG_H


void mylog(char* TAG,void* data);
static void mylog_entry(void *parameter);
int mylog_init(void);

#endif
