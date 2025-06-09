/*
 * Copyright 2021-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "flash_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flash_config"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(BOOT_HEADER_ENABLE) && (BOOT_HEADER_ENABLE == 1)
#if defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".flash_conf"), used))
#elif defined(__ICCARM__)
#pragma location = ".flash_conf"
#endif
const fc_flexspi_nor_config_t flexspi_config = {
    .memConfig =
        {
            .tag                 = FC_BLOCK_TAG,
            .version             = FC_BLOCK_VERSION,
            .readSampleClkSrc    = 0,
            .csHoldTime          = 3,
            .csSetupTime         = 3,
            .deviceModeCfgEnable = 0,
            .configCmdEnable     = 0,
            .deviceType          = 0x1,
            .sflashPadType       = kSerialFlash_1Pads,
            .serialClkFreq       = 1,
            .sflashA1Size        = 0x4000000U,
            .sflashA2Size        = 0,
            .sflashB1Size        = 0,
            .sflashB2Size        = 0,
            .lookupTable =
                {
                    /* 3B Read */
                    [0] = FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x03, FC_RADDR_SDR, FC_FLEXSPI_1PAD, 0x18),
                    [1] = FC_FLEXSPI_LUT_SEQ(FC_READ_SDR, FC_FLEXSPI_1PAD, 0x04, FC_STOP_EXE, FC_FLEXSPI_1PAD, 0x00),

                    /* Read Status */
                    [4 * 1 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x05, FC_READ_SDR, FC_FLEXSPI_1PAD, 0x04),

                    /* Write Enable */
                    [4 * 3 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x06, FC_STOP_EXE, FC_FLEXSPI_1PAD, 0x00),

                    /* 3B Sector erase */
                    [4 * 5 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x20, FC_RADDR_SDR, FC_FLEXSPI_1PAD, 0x18),

                    /* 3B Block erase */
                    [4 * 8 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0xD8, FC_RADDR_SDR, FC_FLEXSPI_1PAD, 0x18),

                    /* 3B Page program */
                    [4 * 9 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x02, FC_RADDR_SDR, FC_FLEXSPI_1PAD, 0x18),
                    [4 * 9 + 1] =
                        FC_FLEXSPI_LUT_SEQ(FC_WRITE_SDR, FC_FLEXSPI_1PAD, 0x00, FC_STOP_EXE, FC_FLEXSPI_1PAD, 0x00),

                    /* chip erase */
                    [4 * 11 + 0] =
                        FC_FLEXSPI_LUT_SEQ(FC_CMD_SDR, FC_FLEXSPI_1PAD, 0x60, FC_STOP_EXE, FC_FLEXSPI_1PAD, 0x00),
                },
        },
    .pageSize           = 0x100,
    .sectorSize         = 0x1000,
    .ipcmdSerialClkFreq = 0,
    .blockSize          = 0x8000,
    .fcb_fill[0]        = 0xFFFFFFFF,
};
#endif /* BOOT_HEADER_ENABLE */
