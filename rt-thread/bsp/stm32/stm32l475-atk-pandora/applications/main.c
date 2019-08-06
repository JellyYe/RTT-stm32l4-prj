/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-6-029     Jelly   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <led_alert.h>
#include <mylog.h>
#include <sensor.h>
#include <key.h>
#include <stdio.h>
#include <string.h>
#include "temp_humi.h"
/*sensor name*/
#define TSL4531_SENSOR_NAME	"li_tsl4531"                                    /*  */
#define AHT10_HUMI_NAME		  "humi_aht10"                                    /*  */
#define AHT10_TEMP_NAME		  "temp_aht10"                                    /*  */
/*消息队列*/
#define MQ_BLOCK_SIZE	RT_ALIGN( sizeof(struct tmp_msg), sizeof(intptr_t) )    /* 为了字节对齐,为了提高mcu的读写效率 */
#define MQ_LEN		(4)
/*创建缓冲区*/
#define RINGBUFFERSIZE      (4069)                  /* ringbuffer缓冲区大小 */
#define THRESHOLD           (RINGBUFFERSIZE / 2)    /* ringbuffer缓冲区阈值 */
/*事件集*/
#define WRITE_EVENT         (0x01U << 0)            /* 感兴趣的事件 */
static rt_event_t recvdata_event;                   /* 事件集 */
static struct rt_ringbuffer *recvdatabuf;           /* ringbuffer */
struct tmp_msg
{
	rt_tick_t	timestamp;
	void		*str_value;
	int		int_value;
	float		float_value;
};


static rt_mq_t tmp_msg_mq;


static void read_lux_entry( void *parameter )
{
	struct tmp_msg		msg;
	struct rt_sensor_data	tsl4531_data;
	rt_uint32_t		lux_data;
	rt_device_t		dev = RT_NULL;
	static rt_size_t	res;

	dev = rt_device_find( parameter );
	if ( dev == RT_NULL )
	{
		rt_kprintf( "Can't find device:%s\n", parameter );
		return;
	}
	if ( rt_device_open( dev, RT_DEVICE_FLAG_RDWR ) != RT_EOK )
	{
		rt_kprintf( "open device failed!\n" );
		return;
	}
/*rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)100);*/

	while ( 1 )
	{
		res = rt_device_read( dev, 0, &tsl4531_data, 1 );
		if ( res != 1 )
		{
			rt_kprintf( "read data failed!size is %d\n", res );
			rt_device_close( dev );
			return;
		}else  {
			lux_data	= tsl4531_data.data.light;
			msg.int_value = lux_data;
			msg.timestamp	= tsl4531_data.timestamp;
			msg.str_value	= parameter;
	    rt_mq_send(tmp_msg_mq, &msg, sizeof msg);
     // rt_kprintf("%s =%d \n",msg.str_value,msg.int_value);
			/*
			 * if(lux_data>200) led_blink();
			 * mylog("lux",&lux_data);
			 * rt_kprintf("lux =%d \n",lux_data);
			 */
		}

		rt_thread_mdelay( 1000 );
	}
}


static void read_temp_humi_entry( void *parameter )
{
	struct tmp_msg		temp_msg,humi_msg;
	rt_device_t		sensor_humi_aht10, sensor_temp_aht10;
	struct rt_sensor_data	aht10_data;
	rt_uint32_t		humi_data, temp_data;
	static rt_size_t	res1, res2;


	/*湿度传感器*/
	sensor_humi_aht10 = rt_device_find( AHT10_HUMI_NAME );
	if ( sensor_humi_aht10 == RT_NULL )
	{
		rt_kprintf( "Can't find sensor_humi_aht10 device:%s\n", AHT10_HUMI_NAME );
		return;
	}
	if ( rt_device_open( sensor_humi_aht10, RT_DEVICE_FLAG_RDWR ) != RT_EOK )
	{
		rt_kprintf( "open sensor_humi_aht10 device failed!\n" );
		return;
	}

	/*温度传感器*/
	sensor_temp_aht10 = rt_device_find( AHT10_TEMP_NAME );
	if ( sensor_temp_aht10 == RT_NULL )
	{
		rt_kprintf( "Can't find sensor_temp_aht10 device:%s\n", AHT10_TEMP_NAME );
		return;
	}
	if ( rt_device_open( sensor_temp_aht10, RT_DEVICE_FLAG_RDWR ) != RT_EOK )
	{
		rt_kprintf( "open sensor_temp_aht10 device failed!\n" );
		return;
	}

	while ( 1 )
	{
		res1 = rt_device_read( sensor_humi_aht10, 0, &aht10_data, 1 );
		if ( res1 != 1 )
		{
			rt_kprintf( "read data failed!size is %d\n", res1 );
			rt_device_close( sensor_humi_aht10 );
			return;
		}else  {
			humi_data = aht10_data.data.humi;
			humi_msg.int_value = humi_data;
			humi_msg.timestamp	= aht10_data.timestamp;
			humi_msg.str_value	= AHT10_HUMI_NAME;
	    rt_mq_send(tmp_msg_mq, &humi_msg, sizeof humi_msg);
			/*
			 * LOG_W("%s:%d", TAG, humi_data);
			 * mylog("humi",&humi_data);
			 */
		}


		res2 = rt_device_read( sensor_temp_aht10, 0, &aht10_data, 1 );
		if ( res2 != 1 )
		{
			rt_kprintf( "read data failed!size is %d\n", res2 );
			rt_device_close( sensor_temp_aht10 );
			return;
		}else    {
			temp_data = aht10_data.data.temp;
			temp_msg.int_value = temp_data;
			temp_msg.timestamp	= aht10_data.timestamp;
			temp_msg.str_value	= AHT10_TEMP_NAME;
	    rt_mq_send(tmp_msg_mq, &temp_msg, sizeof temp_msg);
			if ( temp_data / 10 > 30 )
				led_blink();
			/*
			 * mylog("temp",&temp_data);
			 * rt_kprintf("humi=%d.%d   temp=%d.%d\n",humi_data/10,humi_data%10,temp_data/10,temp_data%10);
			 * LOG_I("aht10 biging");
			 */
		}
		rt_thread_mdelay( 1000 );
	}
}


static void send_sensordata_entry( void *parameter )
{
	 struct tmp_msg msg;
	 static char str_data[100];

	while ( 1 )
	{
		rt_thread_mdelay( 1000 );

		if ( rt_mq_recv( tmp_msg_mq, &msg, sizeof msg, RT_WAITING_FOREVER ) == RT_EOK )
		{
			if ( msg.int_value >= 0 )
			{
				if ( strcmp( msg.str_value, TSL4531_SENSOR_NAME ) == RT_EOK )
				{
				//	rt_kprintf( "[%s] =%d \n", msg.str_value, msg.int_value );
				}

				if ( strcmp( msg.str_value, AHT10_HUMI_NAME ) == RT_EOK )
				{
				//	rt_kprintf( "[%s] data is:%d.%d%\n", msg.str_value, (msg.int_value) / 10, (msg.int_value) % 10 );
				}

				if ( strcmp( msg.str_value, AHT10_TEMP_NAME ) == RT_EOK )
				{
				//	rt_kprintf( "[%s] data is:%d.%d%\n", msg.str_value, (msg.int_value) / 10, (msg.int_value) % 10 );
				}
				
				/*将接收到的数据格式化成字符串，并打印到str_data*/
				/*打印类型取决于被打印参数的类型定义*/
				 sprintf(str_data, "%s,%d\n", msg.str_value, msg.int_value);/*将msg.str_value数据转换成char型， msg.int_value转换成浮点型类型，然后生成字符串str_data*/
				 rt_ringbuffer_put(recvdatabuf, (rt_uint8_t *)str_data, strlen(str_data));/*将接收到的数据存到ringbuffer*/
				 rt_event_send(recvdata_event, WRITE_EVENT);/*发送事件*/
			}
		}
	}
}


static void save_recv_data_entry(void *parameter)
{
    FILE *recvdatafile_p0 = RT_NULL;
    rt_uint32_t set;
    static int writebuffer[1024];
    rt_size_t size;
    while (1)
    {
        /* 接收感兴趣的事件WRITE_EVENT，以永久等待方式去接收 */
        if (rt_event_recv(recvdata_event, WRITE_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &set) != RT_EOK)
        {
            continue;
        }
				
        /*只执行一遍，直到又有事件产生，不然（RT_WAITING_FOREVER）永久等待*/
        do
        {
            /* 接收感兴趣的事件WRITE_EVENT，以1000ms超时方式接收 */
            if (rt_event_recv(recvdata_event, WRITE_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, rt_tick_from_millisecond(1000), &set) == RT_EOK)
            {
                /* 判断写入的数据大小到没到所设置的ringbuffer的阈值 */
                if (rt_ringbuffer_data_len(recvdatabuf) > THRESHOLD)
                {
                    /* 到阈值就直接写数据 */
                    recvdatafile_p0 = fopen("recvdata_p0.csv", "ab+");/*创建文件夹*/
                    if (recvdatafile_p0 != RT_NULL)
                    {
                        while(rt_ringbuffer_data_len(recvdatabuf))
                        {
													  /*取出ringbuffer中的数据到writebuff ，并返回数据长度*/
                            size = rt_ringbuffer_get(recvdatabuf, (rt_uint8_t *)writebuffer, THRESHOLD);
													/*一次写一个字节，共要写入size个字节*/
                            fwrite(writebuffer, 1, size, recvdatafile_p0);/*往文件系统写入数据*/
                        }
                        fclose(recvdatafile_p0);
                    }
                }
                /* 阈值没到就继续接收感兴趣的事件WRITE_EVENT，以1000ms超时方式接收 */
                continue;
            }
            /* 1000ms到了，还没有收到感兴趣的事件，这时候不管到没到阈值，直接写 */
            recvdatafile_p0 = fopen("recvdata_p0.csv", "ab+");
            if (recvdatafile_p0 != RT_NULL)
            {
                while(rt_ringbuffer_data_len(recvdatabuf))
                {
                    size = rt_ringbuffer_get(recvdatabuf, (rt_uint8_t *)writebuffer, THRESHOLD);
                    fwrite(writebuffer, 1, size, recvdatafile_p0);
                }
                fclose(recvdatafile_p0);
            }
        } while(0);
    }
}







int main( void )
{
	int		cnt = 0;
	rt_thread_t	tsl4531_thread, temp_humi_thread,sen_sensor_data_thread,DFS_thread;
	recvdata_event = rt_event_create("temp_evt0", RT_IPC_FLAG_FIFO);
	RT_ASSERT(recvdata_event);
	recvdatabuf = rt_ringbuffer_create(RINGBUFFERSIZE); /* ringbuffer的大小是4KB */
	RT_ASSERT(recvdatabuf);
	
	tmp_msg_mq = rt_mq_create( "temp_mq", MQ_BLOCK_SIZE, MQ_LEN, RT_IPC_FLAG_FIFO );
	/* mylog_init(); */

/*开启tsl4531 lux 检测线程*/
	tsl4531_thread = rt_thread_create( "lux",
					   read_lux_entry,
					   TSL4531_SENSOR_NAME,
					   1024, 6, 10 );
	if ( tsl4531_thread != RT_NULL )
		rt_thread_startup( tsl4531_thread );

/*开启aht10 温湿度 检测线程*/
	temp_humi_thread = rt_thread_create( "temp",
					     read_temp_humi_entry,
					     RT_NULL,
					     1024, 6, 10 );
	if ( temp_humi_thread != RT_NULL )
		rt_thread_startup( temp_humi_thread );

	/*发送数据线程*/
	sen_sensor_data_thread = rt_thread_create( "send_data",
					     send_sensordata_entry,
					     RT_NULL,
					     1024, 6, 10 );
	if ( sen_sensor_data_thread != RT_NULL )
		rt_thread_startup( sen_sensor_data_thread );

	/*文件系统存数据线程*/
//		DFS_thread = rt_thread_create( "DFSsave",
//					     save_recv_data_entry,
//					     RT_NULL,
//					     1024, 6, 10 );
//	if ( DFS_thread != RT_NULL )
//		rt_thread_startup( DFS_thread );
	
	
	led_blink();
	    
}


int rt_hw_aht10_port( void )
{
	struct rt_sensor_config cfg;

	cfg.intf.dev_name	= AHT10_I2C_BUS_NAME;
	cfg.intf.user_data	= (void *) AHT10_ADDR;

	rt_hw_aht10_init( "aht10", &cfg );

	return(RT_EOK);
}


INIT_COMPONENT_EXPORT( rt_hw_aht10_port );
