/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2019-6-25      YE           mylog
*/
#include <rtdevice.h>
#include <rtthread.h>
#include <mylog.h>
#include <rtdbg.h>
#include <string.h>
static struct rt_thread mylog_thread;

//#ifdef MAILBOX
static struct rt_mailbox mb;
static char mb_pool[128];
ALIGN(RT_ALIGN_SIZE)
static char mylog_thread_stack[2048];

struct mb_data_send
{
    char *data_prt;
    rt_uint32_t   mb_data;          
};

struct mb_data_send *mb_data_send_t;
void mylog(char* TAG,void* data)
{	
	mb_data_send_t = (struct mb_data_send*)rt_malloc(sizeof(struct mb_data_send));
	mb_data_send_t->data_prt=TAG;
	mb_data_send_t->mb_data=*((rt_uint32_t*) data);
	rt_mb_send(&mb,(rt_ubase_t)mb_data_send_t);
}


static void mylog_entry(void *parameter)
{
   
	struct mb_data_send *pstr;
	
	//LOG_W("1.TYPE:%d", rt_object_get_type(&mb.parent.parent));
    while (1)
    {
        //rt_kprintf("thread1: try to recv a mail\n");
			  

        /* 从邮箱中收取邮件 */
			/*这里(rt_ubase_t *)&pstr & 的作用是取结构体指针的地址，然后用邮箱接收到发送消息的结构体指针的地址 ，指向pstr结构体指针的地址，这里要搞清楚的就是结构体指针地址跟邮箱接收消息的地址问题*/
        if (rt_mb_recv(&mb, (rt_ubase_t *)&pstr, RT_WAITING_FOREVER) == RT_EOK)
        {
					//	mylog("main",  &str->mb_data );
					if(strcmp(pstr->data_prt,"humi")==RT_EOK)
						{
							rt_kprintf("[%s] data is:%d.%d%\n",pstr->data_prt , (pstr->mb_data)/10,(pstr->mb_data)%10);
						}
		  		else if(strcmp(pstr->data_prt,"temp")==RT_EOK)
						{
							rt_kprintf("[%s] data is:%d.%d\n",pstr->data_prt , (pstr->mb_data)/10,(pstr->mb_data)%10);
						}
		  		else if(strcmp(pstr->data_prt,"lux")==RT_EOK)
						{
							rt_kprintf("[%s] data is:%dlux\n",pstr->data_prt , pstr->mb_data);
						}
		  		else
						{
							rt_kprintf("no sensor data\n");
						}

        }
    }
		/* 在接收线程处理完毕后，需要释放相应的内存块 */
		 rt_free(pstr);
    /* 执行邮箱对象脱离 */
    rt_mb_detach(&mb);
}

int mylog_init(void)
{
    rt_err_t result;


    /* 初始化一个 mailbox */
    result = rt_mb_init(&mb,
                        "mbt",                      /* 名称是 mbt */
                        &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
        return -1;
    }

    rt_thread_init(&mylog_thread,
                   "mylog_thread",
                   mylog_entry,
                   RT_NULL,
                   &mylog_thread_stack[0],
                   sizeof(mylog_thread_stack),
                  7, 10);
    rt_thread_startup(&mylog_thread);


    return 0;
}

//#endif
