/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    board.h
 * @brief   Board initialization header file.
 */
 
/* This is a template for board specific configuration created by MCUXpresso IDE Project Wizard.*/

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

#if defined(__cplusplus)
}
#endif /* __cplusplus */


#ifndef BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_BUILTIN_GPIO GPIOC
#endif

/* Blink */
#define BOARD_LED_GPIO     BOARD_LED_BUILTIN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_INITPINS_LED_BUILTIN_PIN

/* FreeRTOS UART Debug */
#define DEBUG_UART            UART1
#define DEBUG_UART_CLKSRC     SYS_CLK
#define DEBUG_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define DEBUG_UART_RX_TX_IRQn UART1_RX_TX_IRQn

/* FreeRTOS UART IMU */
#define IMU_UART            UART0
#define IMU_UART_CLKSRC     SYS_CLK
#define IMU_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define IMU_UART_RX_TX_IRQn UART0_RX_TX_IRQn

/* FreeRTOS UART Radio */
#define RADIO_UART            UART3
#define RADIO_UART_CLKSRC     SYS_CLK
#define RADIO_UART_CLK_FREQ   CLOCK_GetFreq(SYS_CLK)
#define RADIO_UART_RX_TX_IRQn UART3_RX_TX_IRQn

#define CAN_CLK_SOURCE (kFLEXCAN_ClkSrc1)
#define CAN_CLK_FREQ   CLOCK_GetFreq(kCLOCK_BusClk)

/* DSPI Constants */
#define EXAMPLE_DSPI_MASTER_BASE (SPI0_BASE) //changed example code from SPI1 to SPI0
#define EXAMPLE_DSPI_MASTER_IRQN (SPI0_IRQn)
#define DSPI_MASTER_CLK_SRC      (DSPI0_CLK_SRC)
#define DSPI_MASTER_CLK_FREQ     CLOCK_GetFreq((DSPI0_CLK_SRC))
#define EXAMPLE_DSPI_MASTER_BASEADDR ((SPI_Type *)EXAMPLE_DSPI_MASTER_BASE)
#define TRANSFER_SIZE     (4)     /*! Transfer size */
#define TRANSFER_BAUDRATE (7000U) /*! Transfer baudrate - 7k */

#endif /* _BOARD_H_ */
