/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v8.0
processor: MK66FX1M0xxx18
package_id: MK66FX1M0VLQ18
mcu_data: ksdk2_0
processor_version: 8.0.1
pin_labels:
- {pin_num: '110', pin_signal: PTC5/LLWU_P9/SPI0_SCK/LPTMR0_ALT2/I2S0_RXD0/FB_AD10/SDRAM_A18/CMP0_OUT/FTM0_CH2, label: LED_RGBBUILTIN, identifier: LED_BUILTIN}
- {pin_num: '95', pin_signal: TSI0_CH9/PTB16/SPI1_SOUT/UART0_RX/FTM_CLKIN0/FB_AD17/SDRAM_D17/EWM_IN/TPM_CLKIN0, label: DEBUG_UART_RX, identifier: DEBUG_UART_RX;DEBUG_UART_TX}
- {pin_num: '96', pin_signal: TSI0_CH10/PTB17/SPI1_SIN/UART0_TX/FTM_CLKIN1/FB_AD16/SDRAM_D16/EWM_OUT_b/TPM_CLKIN1, label: DEBUG_UART_TX, identifier: DEBUG_UART_TX;DEBUG_UART_RX}
- {pin_num: '106', pin_signal: CMP1_IN1/PTC3/LLWU_P7/SPI0_PCS1/UART1_RX/FTM0_CH2/CLKOUT/I2S0_TX_BCLK, label: IMU_UART_TX, identifier: IMU_UART_TX;IMU_UART_RX}
- {pin_num: '109', pin_signal: PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/FTM0_CH3/FB_AD11/SDRAM_A19/CMP1_OUT, label: IMU_UART_RX, identifier: IMU_UART_RX;IMU_UART_TX}
- {pin_num: '129', pin_signal: PTD2/LLWU_P13/SPI0_SOUT/UART2_RX/FTM3_CH2/FB_AD4/SDRAM_A12/I2C0_SCL, label: RADIO_UART_RX, identifier: RADIO_UART_RX}
- {pin_num: '130', pin_signal: PTD3/SPI0_SIN/UART2_TX/FTM3_CH3/FB_AD3/SDRAM_A11/I2C0_SDA, label: RADIO_UART_TX, identifier: RADIO_UART_TX}
- {pin_num: '47', pin_signal: PTE26/ENET_1588_CLKIN/UART4_CTS_b/RTC_CLKOUT/USB0_CLKIN, label: CAN1_STBN, identifier: CAN1_STBN}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '110', peripheral: GPIOC, signal: 'GPIO, 5', pin_signal: PTC5/LLWU_P9/SPI0_SCK/LPTMR0_ALT2/I2S0_RXD0/FB_AD10/SDRAM_A18/CMP0_OUT/FTM0_CH2, direction: OUTPUT}
  - {pin_num: '95', peripheral: UART0, signal: RX, pin_signal: TSI0_CH9/PTB16/SPI1_SOUT/UART0_RX/FTM_CLKIN0/FB_AD17/SDRAM_D17/EWM_IN/TPM_CLKIN0, identifier: DEBUG_UART_TX}
  - {pin_num: '96', peripheral: UART0, signal: TX, pin_signal: TSI0_CH10/PTB17/SPI1_SIN/UART0_TX/FTM_CLKIN1/FB_AD16/SDRAM_D16/EWM_OUT_b/TPM_CLKIN1, identifier: DEBUG_UART_RX}
  - {pin_num: '106', peripheral: UART1, signal: RX, pin_signal: CMP1_IN1/PTC3/LLWU_P7/SPI0_PCS1/UART1_RX/FTM0_CH2/CLKOUT/I2S0_TX_BCLK, identifier: IMU_UART_RX}
  - {pin_num: '109', peripheral: UART1, signal: TX, pin_signal: PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/FTM0_CH3/FB_AD11/SDRAM_A19/CMP1_OUT, identifier: IMU_UART_TX}
  - {pin_num: '4', peripheral: SDHC, signal: CMD, pin_signal: ADC1_SE7a/PTE3/SPI1_SIN/UART1_RTS_b/SDHC0_CMD/TRACE_D1/SPI1_SOUT, pull_select: up, pull_enable: enable}
  - {pin_num: '2', peripheral: SDHC, signal: 'DATA, 0', pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN, pull_select: up,
    pull_enable: enable}
  - {pin_num: '1', peripheral: SDHC, signal: 'DATA, 1', pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT, pull_select: up,
    pull_enable: enable}
  - {pin_num: '8', peripheral: SDHC, signal: 'DATA, 2', pin_signal: PTE5/SPI1_PCS2/UART3_RX/SDHC0_D2/FTM3_CH0, pull_select: up, pull_enable: enable}
  - {pin_num: '7', peripheral: SDHC, signal: 'DATA, 3', pin_signal: PTE4/LLWU_P2/SPI1_PCS0/UART3_TX/SDHC0_D3/TRACE_D0, pull_select: up, pull_enable: enable}
  - {pin_num: '3', peripheral: SDHC, signal: DCLK, pin_signal: ADC1_SE6a/PTE2/LLWU_P1/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/TRACE_D2, pull_select: up, pull_enable: enable}
  - {pin_num: '45', peripheral: CAN1, signal: TX, pin_signal: ADC0_SE17/PTE24/CAN1_TX/UART4_TX/I2C0_SCL/EWM_OUT_b}
  - {pin_num: '46', peripheral: CAN1, signal: RX, pin_signal: ADC0_SE18/PTE25/LLWU_P21/CAN1_RX/UART4_RX/I2C0_SDA/EWM_IN}
  - {pin_num: '47', peripheral: GPIOE, signal: 'GPIO, 26', pin_signal: PTE26/ENET_1588_CLKIN/UART4_CTS_b/RTC_CLKOUT/USB0_CLKIN, direction: OUTPUT, gpio_init_state: 'true'}
  - {pin_num: '123', peripheral: UART3, signal: RX, pin_signal: PTC16/CAN1_RX/UART3_RX/ENET0_1588_TMR0/FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b/SDRAM_DQM2}
  - {pin_num: '124', peripheral: UART3, signal: TX, pin_signal: PTC17/CAN1_TX/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/SDRAM_DQM3}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    gpio_pin_config_t LED_BUILTIN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTC5 (pin 110)  */
    GPIO_PinInit(BOARD_INITPINS_LED_BUILTIN_GPIO, BOARD_INITPINS_LED_BUILTIN_PIN, &LED_BUILTIN_config);

    gpio_pin_config_t CAN1_STBN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };
    /* Initialize GPIO functionality on pin PTE26 (pin 47)  */
    GPIO_PinInit(BOARD_INITPINS_CAN1_STBN_GPIO, BOARD_INITPINS_CAN1_STBN_PIN, &CAN1_STBN_config);

    /* PORTB16 (pin 95) is configured as UART0_RX */
    PORT_SetPinMux(BOARD_INITPINS_DEBUG_UART_TX_PORT, BOARD_INITPINS_DEBUG_UART_TX_PIN, kPORT_MuxAlt3);

    /* PORTB17 (pin 96) is configured as UART0_TX */
    PORT_SetPinMux(BOARD_INITPINS_DEBUG_UART_RX_PORT, BOARD_INITPINS_DEBUG_UART_RX_PIN, kPORT_MuxAlt3);

    /* PORTC16 (pin 123) is configured as UART3_RX */
    PORT_SetPinMux(PORTC, 16U, kPORT_MuxAlt3);

    /* PORTC17 (pin 124) is configured as UART3_TX */
    PORT_SetPinMux(PORTC, 17U, kPORT_MuxAlt3);

    /* PORTC3 (pin 106) is configured as UART1_RX */
    PORT_SetPinMux(BOARD_INITPINS_IMU_UART_RX_PORT, BOARD_INITPINS_IMU_UART_RX_PIN, kPORT_MuxAlt3);

    /* PORTC4 (pin 109) is configured as UART1_TX */
    PORT_SetPinMux(BOARD_INITPINS_IMU_UART_TX_PORT, BOARD_INITPINS_IMU_UART_TX_PIN, kPORT_MuxAlt3);

    /* PORTC5 (pin 110) is configured as PTC5 */
    PORT_SetPinMux(BOARD_INITPINS_LED_BUILTIN_PORT, BOARD_INITPINS_LED_BUILTIN_PIN, kPORT_MuxAsGpio);

    /* PORTE0 (pin 1) is configured as SDHC0_D1 */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt4);

    PORTE->PCR[0] = ((PORTE->PCR[0] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTE1 (pin 2) is configured as SDHC0_D0 */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt4);

    PORTE->PCR[1] = ((PORTE->PCR[1] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTE2 (pin 3) is configured as SDHC0_DCLK */
    PORT_SetPinMux(PORTE, 2U, kPORT_MuxAlt4);

    PORTE->PCR[2] = ((PORTE->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTE24 (pin 45) is configured as CAN1_TX */
    PORT_SetPinMux(PORTE, 24U, kPORT_MuxAlt2);

    /* PORTE25 (pin 46) is configured as CAN1_RX */
    PORT_SetPinMux(PORTE, 25U, kPORT_MuxAlt2);

    /* PORTE26 (pin 47) is configured as PTE26 */
    PORT_SetPinMux(BOARD_INITPINS_CAN1_STBN_PORT, BOARD_INITPINS_CAN1_STBN_PIN, kPORT_MuxAsGpio);

    /* PORTE3 (pin 4) is configured as SDHC0_CMD */
    PORT_SetPinMux(PORTE, 3U, kPORT_MuxAlt4);

    PORTE->PCR[3] = ((PORTE->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTE4 (pin 7) is configured as SDHC0_D3 */
    PORT_SetPinMux(PORTE, 4U, kPORT_MuxAlt4);

    PORTE->PCR[4] = ((PORTE->PCR[4] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    /* PORTE5 (pin 8) is configured as SDHC0_D2 */
    PORT_SetPinMux(PORTE, 5U, kPORT_MuxAlt4);

    PORTE->PCR[5] = ((PORTE->PCR[5] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | (uint32_t)(kPORT_PullUp));

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK | SIM_SOPT5_UART1TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX)

                  /* UART 1 transmit data source select: UART1_TX pin. */
                  | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
