/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-27     ZYLX              the first version
 */

#include <drivers/pin.h>
#include <enc28j60.h>
#include "drv_spi.h"
#include "board.h"

int enc28j60_init(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();                                                               /*1.���жϹܽ�ʱ��*/
    rt_hw_spi_device_attach("spi2", "spi21", GPIOD, GPIO_PIN_5);                                /*2.��spi21���豸���ص�spi2�����ϣ����ҳ�ʼ��cs�ܽ�*/

    /* attach enc28j60 to spi. spi21 cs - PD6 */
    enc28j60_attach("spi21");                                                                   /*3.��ʼ��enc28j60�豸����spi21�豸*/

    /* init interrupt pin */
    rt_pin_mode(PIN_NRF_IRQ, PIN_MODE_INPUT_PULLUP);                                            /*4.�����жϹܽ�ģʽ*/
    rt_pin_attach_irq(PIN_NRF_IRQ, PIN_IRQ_MODE_FALLING, (void(*)(void*))enc28j60_isr, RT_NULL);/*5.��enc28j60_isr�жϻص������󶨵��жϹܽ�*/
    rt_pin_irq_enable(PIN_NRF_IRQ, PIN_IRQ_ENABLE);                                             /*6.ʹ���жϹܽ��ж�*/

    return 0;
}
INIT_COMPONENT_EXPORT(enc28j60_init);
