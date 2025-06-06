/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    board.h
 * @brief   Board initialization header file.
 */

/* This is an empty template for board specific configuration.*/

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * @brief	The board name
 */
#define BOARD_NAME "board"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 	Initialize board specific settings.
 */
void BOARD_InitDebugConsole(void);
void BOARD_ClockPreConfig(void);
void BOARD_ClockPostConfig(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
