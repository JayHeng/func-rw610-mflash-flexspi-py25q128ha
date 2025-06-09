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

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    status_t status;

    BOARD_InitHardware();
    
    PRINTF("LFS basic test \r\n");

    lfs_get_default_config(&cfg);

    status = lfs_storage_init(&cfg);
    if (status != kStatus_Success)
    {
        PRINTF("LFS storage init failed: %i\r\n", status);
        return status;
    }

    int res = lfs_format(&lfs, &cfg);
    if (res)
    {
        PRINTF("\rError formatting LFS: %d\r\n", res);
        return -1;
    }

    res = lfs_mount(&lfs, &cfg);
    if (res)
    {
        PRINTF("\rError mounting LFS\r\n");
        return -1;
    }

    {
        char *dirpath;
        dirpath = "/";
        res = lfs_mkdir(&lfs, dirpath);
        if (res)
        {
            PRINTF("\rError creating directory: %i\r\n", res);
            return -1;
        }

        {
            lfs_file_t file;
            char *filepath;
            filepath = "test.txt";
            res = lfs_file_open(&lfs, &file, filepath, LFS_O_WRONLY | LFS_O_APPEND | LFS_O_CREAT);
            if (res)
            {
                PRINTF("\rError opening file: %i\r\n", res);
                return -1;
            }
            char *filecontent;
            filecontent = "hello world";
            res = lfs_file_write(&lfs, &file, filecontent, strlen(filecontent));
            if (res > 0)
                res = lfs_file_write(&lfs, &file, "\r\n", 2);

            if (res < 0)
            {
                PRINTF("\rError writing file: %i\r\n", res);
            }

            res = lfs_file_close(&lfs, &file);
            if (res)
            {
                PRINTF("\rError closing file: %i\r\n", res);
            }
        }
    }

    while (1)
    {

    }
}
