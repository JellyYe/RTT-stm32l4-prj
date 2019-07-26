/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-14      YE           temp_humi
*/
/*
 * 程序清单：这是一个 I2C 设备使用例程
 * 例程导出了 i2c_aht10_sample 命令到控制终端
 * 命令调用格式：i2c_aht10_sample i2c1
 * 命令解释：命令第二个参数是要使用的I2C总线设备名称，为空则使用默认的I2C总线设备
 * 程序功能：通过 I2C 设备读取温湿度传感器 aht10 的温湿度数据并打印
*/

#include <rtthread.h>
#include <rtdevice.h>
#include "sensor.h"
#include "temp_humi.h"
#include <rtdbg.h>

#define SENSOR_TEMP_RANGE_MAX (85)
#define SENSOR_TEMP_RANGE_MIN (-40)
#define SENSOR_HUMI_RANGE_MAX (100)
#define SENSOR_HUMI_RANGE_MIN (0)

/* 写传感器寄存器 */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[3];
    struct rt_i2c_msg msgs;

    buf[0] = reg; //cmd
    buf[1] = data[0];
    buf[2] = data[1];

    msgs.addr = AHT10_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 3;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
			 
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/* 读传感器寄存器数据 */
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = AHT10_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
			  
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static float aht10_read_temperature(aht10_device_t dev)
{
    rt_uint8_t temp[6];
    float cur_temp = -50.0;  //The data is error with missing measurement.  
    rt_err_t result;

    RT_ASSERT(dev);

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        rt_uint8_t cmd[2] = {0, 0};
        write_reg(dev->i2c, AHT10_GET_DATA, cmd); // sample data cmd

       // result = calibration_enabled(dev);
        if (result != RT_EOK)
        {
            rt_thread_mdelay(1500);
            sensor_init(dev); // reset sensor
            LOG_E("The aht10 is under an abnormal status. Please try again");
        }
        else
        {
            read_regs(dev->i2c, 6, temp); // get data
            /*sensor temperature converse to reality */
            cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;
        }
    }
    else
    {
        LOG_E("The aht10 could not respond temperature measurement at this time. Please try again");
    }
    rt_mutex_release(dev->lock);

    return cur_temp;
}

static float aht10_read_humidity(aht10_device_t dev)
{
    rt_uint8_t temp[6];
    float cur_humi = 0.0;  //The data is error with missing measurement.  
    rt_err_t result;

    RT_ASSERT(dev);

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        rt_uint8_t cmd[2] = {0, 0};
        write_reg(dev->i2c, AHT10_GET_DATA, cmd); // sample data cmd

        //result = calibration_enabled(dev);
        if (result != RT_EOK)
        {
            rt_thread_mdelay(1500);
            sensor_init(dev);
            LOG_E("The aht10 is under an abnormal status. Please try again");
        }
        else
        {
            read_regs(dev->i2c, 6, temp);                                                          // get data
            cur_humi = (temp[1] << 12 | temp[2] << 4 | (temp[3] & 0xf0) >> 4) * 100.0 / (1 << 20); //sensor humidity converse to reality
        }
    }
    else
    {
        LOG_E("The aht10 could not respond temperature measurement at this time. Please try again");
    }
    rt_mutex_release(dev->lock);

    return cur_humi;
}

/* 传感器设备初始化 */
static rt_err_t sensor_init(aht10_device_t dev)
{
    rt_uint8_t temp[2] = {0, 0};

    write_reg(dev->i2c, AHT10_NORMAL_CMD, temp);
    rt_thread_delay(rt_tick_from_millisecond(500)); //at least 300 ms

    temp[0] = 0x08;
    temp[1] = 0x00;
    write_reg(dev->i2c, AHT10_CALIBRATION_CMD, temp); //go into calibration
    rt_thread_delay(rt_tick_from_millisecond(450));   //at least 300 ms

    return RT_EOK;
}

/*此函数初始化aht10注册设备驱动程序*/
 aht10_device_t aht10_init(const char *i2c_bus_name)
{
    aht10_device_t dev;

    RT_ASSERT(i2c_bus_name);

    dev = rt_calloc(1, sizeof(struct aht10_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for aht10 device on '%s' ", i2c_bus_name);
        return RT_NULL;
    }

    dev->i2c = rt_i2c_bus_device_find(i2c_bus_name);//获取设备句柄
    if (dev->i2c == RT_NULL)
    {
        LOG_E("Can't find aht10 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }
    dev->lock = rt_mutex_create("mutex_aht10", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for aht10 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }
    sensor_init(dev);

    return dev;
}

/*********以上是传感器硬件设备驱动部分*****************/






/*********以下是sensor设备驱动部分*****************/

static struct aht10_device *temp_humi_dev;

static rt_err_t _aht10_init(struct rt_sensor_intf *intf)
{
    temp_humi_dev = aht10_init(intf->dev_name);

    if (temp_humi_dev == RT_NULL)
    {
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

void aht10_deinit(aht10_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);

#ifdef AHT10_USING_SOFT_FILTER
    rt_thread_delete(dev->thread);
#endif

    rt_free(dev);
}

static rt_size_t _aht10_polling_get_data(rt_sensor_t sensor, struct rt_sensor_data *data)
{
    float temperature_x10, humidity_x10;
    
    if (sensor->info.type == RT_SENSOR_CLASS_TEMP)
    {
			temperature_x10 = 10 * aht10_read_temperature(temp_humi_dev);//因为要让读出来的数据有到小数点后一位
        data->data.temp = (rt_int32_t)temperature_x10;
        data->timestamp = rt_sensor_get_ts();
    }    
    else if (sensor->info.type == RT_SENSOR_CLASS_HUMI)
    {
        humidity_x10    = 10 * aht10_read_humidity(temp_humi_dev);
        data->data.humi = (rt_int32_t)humidity_x10;
        data->timestamp = rt_sensor_get_ts();
    }
    return 1;
}
//MSH_CMD_EXPORT(_aht10_polling_get_data , say hello to RT-Thread);
static rt_size_t aht10_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    RT_ASSERT(buf);

    if (sensor->config.mode == RT_SENSOR_MODE_POLLING)
    {
        return _aht10_polling_get_data(sensor, buf);
    }
    else
        return 0;
}

static rt_err_t aht10_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    rt_err_t result = RT_EOK;

    return result;
}

static struct rt_sensor_ops sensor_ops =//完成两个回调函数就可以读取数据了
{
    aht10_fetch_data,
    aht10_control
};

int rt_hw_aht10_init(const char *name, struct rt_sensor_config *cfg)
{
    rt_int8_t result;
    rt_sensor_t sensor_temp = RT_NULL, sensor_humi = RT_NULL;
    
// #ifdef PKG_USING_AHT10   
    
     /* temperature sensor register */
    sensor_temp = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor_temp == RT_NULL)
        return -1;

    sensor_temp->info.type       = RT_SENSOR_CLASS_TEMP;
    sensor_temp->info.vendor     = RT_SENSOR_VENDOR_UNKNOWN;
    sensor_temp->info.model      = "aht10";
    sensor_temp->info.unit       = RT_SENSOR_UNIT_DCELSIUS;
    sensor_temp->info.intf_type  = RT_SENSOR_INTF_I2C;
    sensor_temp->info.range_max  = SENSOR_TEMP_RANGE_MAX;
    sensor_temp->info.range_min  = SENSOR_TEMP_RANGE_MIN;
    sensor_temp->info.period_min = 5;

    rt_memcpy(&sensor_temp->config, cfg, sizeof(struct rt_sensor_config));
    sensor_temp->ops = &sensor_ops;

    result = rt_hw_sensor_register(sensor_temp, name, RT_DEVICE_FLAG_RDONLY, RT_NULL);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        goto __exit;
    }
    
    /* humidity sensor register */
    sensor_humi = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor_humi == RT_NULL)
        return -1;

    sensor_humi->info.type       = RT_SENSOR_CLASS_HUMI;
    sensor_humi->info.vendor     = RT_SENSOR_VENDOR_UNKNOWN;
    sensor_humi->info.model      = "aht10";
    sensor_humi->info.unit       = RT_SENSOR_UNIT_PERMILLAGE;
    sensor_humi->info.intf_type  = RT_SENSOR_INTF_I2C;
    sensor_humi->info.range_max  = SENSOR_HUMI_RANGE_MAX;
    sensor_humi->info.range_min  = SENSOR_HUMI_RANGE_MIN;
    sensor_humi->info.period_min = 5;

    rt_memcpy(&sensor_humi->config, cfg, sizeof(struct rt_sensor_config));
    sensor_humi->ops = &sensor_ops;

    result = rt_hw_sensor_register(sensor_humi, name, RT_DEVICE_FLAG_RDONLY, RT_NULL);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        goto __exit;
    }
    
// #endif
    
    _aht10_init(&cfg->intf);
    return RT_EOK;
    
__exit:
    if (sensor_temp)
        rt_free(sensor_temp);
    if (sensor_humi)
        rt_free(sensor_humi);
    if (temp_humi_dev)
        aht10_deinit(temp_humi_dev);
    return -RT_ERROR;     
}


