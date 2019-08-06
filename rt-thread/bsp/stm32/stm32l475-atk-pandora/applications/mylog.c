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
			  

        /* ����������ȡ�ʼ� */
			/*����(rt_ubase_t *)&pstr & ��������ȡ�ṹ��ָ��ĵ�ַ��Ȼ����������յ�������Ϣ�Ľṹ��ָ��ĵ�ַ ��ָ��pstr�ṹ��ָ��ĵ�ַ������Ҫ������ľ��ǽṹ��ָ���ַ�����������Ϣ�ĵ�ַ����*/
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
		/* �ڽ����̴߳�����Ϻ���Ҫ�ͷ���Ӧ���ڴ�� */
		 rt_free(pstr);
    /* ִ������������� */
    rt_mb_detach(&mb);
}

int mylog_init(void)
{
    rt_err_t result;


    /* ��ʼ��һ�� mailbox */
    result = rt_mb_init(&mb,
                        "mbt",                      /* ������ mbt */
                        &mb_pool[0],                /* �����õ����ڴ���� mb_pool */
                        sizeof(mb_pool) / 4,        /* �����е��ʼ���Ŀ����Ϊһ���ʼ�ռ 4 �ֽ� */
                        RT_IPC_FLAG_FIFO);          /* ���� FIFO ��ʽ�����̵߳ȴ� */
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
