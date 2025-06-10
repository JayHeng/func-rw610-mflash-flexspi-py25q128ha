/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "app.h"
#include "fsl_debug_console.h"
#include "lfs.h"
#include "lfs_mflash.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

lfs_t lfs;
struct lfs_config cfg;

uint32_t s_wr_buf[64];
uint32_t s_rb_buf[64];

/*******************************************************************************
 * Code
 ******************************************************************************/

void mflash_drv_self_test(void)
{
    PRINTF("mflash_drv self test \r\n");
    mflash_drv_init();
    mflash_drv_sector_erase(0x0);
    for (uint32_t i=0; i<64; i++)
    {
        s_wr_buf[i] = i;
    }
    mflash_drv_page_program(0x0, s_wr_buf);
    mflash_drv_read(0x0, s_rb_buf, sizeof(s_wr_buf));
    PRINTF("init/erase/program/read test ");
    if (!memcmp(s_rb_buf, s_wr_buf, sizeof(s_wr_buf)))
    {
        PRINTF("pass\r\n");
    }
    else
    {
        PRINTF("fail\r\n");
        while (1);
    }
}

int main(void)
{
    status_t status;

    BOARD_InitHardware();
    
    mflash_drv_self_test();
    
    PRINTF("LFS basic test \r\n");

    lfs_get_default_config(&cfg);

    status = lfs_storage_init(&cfg);
    PRINTF("LFS storage init ");
    if (status != kStatus_Success)
    {
        PRINTF("failed: %i\r\n", status);
        return status;
    }

    PRINTF("done\r\nformatting LFS ");
    int res = lfs_format(&lfs, &cfg);
    if (res)
    {
        PRINTF("Error: %d\r\n", res);
        return -1;
    }

    PRINTF("done\r\nmounting LFS ");
    res = lfs_mount(&lfs, &cfg);
    if (res)
    {
        PRINTF("Error: %d\r\n", res);
        return -1;
    }

    {
        char *dirpath;
        dirpath = "/";
        res = lfs_mkdir(&lfs, dirpath);
        PRINTF("done\r\ncreating directory ");
        if (res)
        {
            PRINTF("Error: %i\r\n", res);
            return -1;
        }

        {
            lfs_file_t file;
            char *filepath;
            filepath = "test.txt";
            res = lfs_file_open(&lfs, &file, filepath, LFS_O_WRONLY | LFS_O_APPEND | LFS_O_CREAT);
            PRINTF("done\r\nopening file ");
            if (res)
            {
                PRINTF("Error: %i\r\n", res);
                return -1;
            }
            char *filecontent;
            filecontent = "hello world";
            res = lfs_file_write(&lfs, &file, filecontent, strlen(filecontent));
            PRINTF("done\r\nwriting file ");
            if (res > 0)
                res = lfs_file_write(&lfs, &file, "\r\n", 2);

            if (res < 0)
            {
                PRINTF("Error: %i\r\n", res);
            }
            PRINTF("done\r\nclosing file ");
            res = lfs_file_close(&lfs, &file);
            if (res)
            {
                PRINTF("Error: %i\r\n", res);
            }
            PRINTF("done\r\n");
        }
    }

    while (1)
    {

    }
}
