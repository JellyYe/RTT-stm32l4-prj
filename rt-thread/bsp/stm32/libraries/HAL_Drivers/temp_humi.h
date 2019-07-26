/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           key
*/

#ifndef TEMP_HUMI
#define TEMP_HUMI
#include <rtthread.h>
#include <sensor.h>
#define AHT10_I2C_BUS_NAME          "i2c4"  /* 传感器连接的I2C总线设备名称 */
#define AHT10_ADDR                  0x38    /* 从机地址 */
#define AHT10_CALIBRATION_CMD       0xE1    /* 校准命令 */
#define AHT10_NORMAL_CMD            0xA8    /* 一般命令 */
#define AHT10_GET_DATA              0xAC    /* 获取数据命令 */

struct aht10_device
{
    struct rt_i2c_bus_device *i2c;

#ifdef AHT10_USING_SOFT_FILTER
    filter_data_t temp_filter;
    filter_data_t humi_filter;

    rt_thread_t thread;
    rt_uint32_t period; //sample period
#endif /* AHT10_USING_SOFT_FILTER */

    rt_mutex_t lock;
};
typedef struct aht10_device *aht10_device_t;

rt_err_t i2c_aht10_sample_thread(void);
int rt_hw_aht10_init(const char *name, struct rt_sensor_config *cfg);
static rt_err_t sensor_init(aht10_device_t dev);

#endif
