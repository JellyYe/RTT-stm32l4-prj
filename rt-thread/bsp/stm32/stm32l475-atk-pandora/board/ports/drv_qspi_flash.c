/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     zylx         first version
 */
 
#include <board.h>
#include <drv_qspi.h>
#include <rtdevice.h>
#include <rthw.h>
#include <finsh.h>

#ifdef BSP_USING_QSPI_FLASH

#include "spi_flash.h"
#include "spi_flash_sfud.h"

char w25qxx_read_status_register2(struct rt_qspi_device *device)
{
    /* 0x35 read status register2 */
    char instruction = 0x35, status;

    rt_qspi_send_then_recv(device, &instruction, 1, &status, 1);

    return status;
}

void w25qxx_write_enable(struct rt_qspi_device *device)
{
    /* 0x06 write enable */
    char instruction = 0x06;

    rt_qspi_send(device, &instruction, 1);
}

void w25qxx_enter_qspi_mode(struct rt_qspi_device *device)
{
    char status = 0;
    /* 0x38 enter qspi mode */
    char instruction = 0x38;
    char write_status2_buf[2] = {0};

    /* 0x31 write status register2 */
    write_status2_buf[0] = 0x31;

    status = w25qxx_read_status_register2(device);
    if (!(status & 0x02))
    {
        status |= 1 << 1;
        w25qxx_write_enable(device);
        write_status2_buf[1] = status;
        rt_qspi_send(device, &write_status2_buf, 2);
        rt_qspi_send(device, &instruction, 1);
        rt_kprintf("flash already enter qspi mode\n");
        rt_thread_mdelay(10);
    }
}

/*将w25q128注册成块设备，才能使用elmfatfs进行文件操作*/
static int rt_hw_qspi_flash_with_sfud_init(void)
{
    stm32_qspi_bus_attach_device("qspi1", "qspi10", RT_NULL, 4, w25qxx_enter_qspi_mode, RT_NULL);
    
    /* init w25q128 */
    if (RT_NULL == rt_sfud_flash_probe("W25Q128", "qspi10"))
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_qspi_flash_with_sfud_init);


#if defined(RT_USING_DFS_ELMFAT) && !defined(BSP_USING_SDCARD)
#include <dfs_fs.h>

#define BLK_DEV_NAME  "W25Q128"

int mnt_init(void)
{
    rt_thread_delay(RT_TICK_PER_SECOND);

    if (dfs_mount(BLK_DEV_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    }
    else
    {
        if(dfs_mkfs("elm", BLK_DEV_NAME) == 0)
        {
            if (dfs_mount(BLK_DEV_NAME, "/", "elm", 0, 0) == 0)
            {
                rt_kprintf("file system initialization done!\n");
            }
            else
            {
                rt_kprintf("file system initialization failed!\n");
            }
        }
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);
#endif /* defined(RT_USING_DFS_ELMFAT) && !defined(BSP_USING_SDCARD) */




#ifdef PKG_USING_LITTLEFS
/* 添加 fal 头文件 */
#include <fal.h>
/* 添加文件系统头文件 */
#include <dfs_fs.h>
/* 添加 DEBUG 头文件 */
#define DBG_SECTION_NAME               "drv_qspi"
#define DBG_LEVEL                      DBG_INFO
#include <rtdbg.h>
/* 定义要使用的分区名字 */
//#define FS_PARTITION_NAME              "app"   
#define FS_PARTITION_NAME              "filesystem"   

int littlefs_init(void)
{
    struct rt_device *mtd_dev = RT_NULL;
    /* 初始化 fal */    
    fal_init();
    /* 生成 mtd 设备 */
    mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
    }
    else
    {
			LOG_I("create a mtd device successful!");
        /* 挂载 littlefs */
        if (dfs_mount(FS_PARTITION_NAME, "/", "lfs", 0, 0) == 0)
        {
            LOG_I("Filesystem initialized!");
        }
        else
        {
					
            LOG_I("monut littefs successful!");
            /* 格式化文件系统 */
            dfs_mkfs("lfs", FS_PARTITION_NAME);
            /* 挂载 littlefs */
            if (dfs_mount("filesystem", "/", "lfs", 0, 0) == 0)
            {
                LOG_I("Filesystem initialized!");
            }
            else
            {
                LOG_E("Failed to initialize filesystem!");
            }
        }
    }
		 return 0;
}

INIT_ENV_EXPORT(littlefs_init);

#endif /* PKG_USING_LITTLEFS */


#endif /* BSP_USING_QSPI_FLASH */
