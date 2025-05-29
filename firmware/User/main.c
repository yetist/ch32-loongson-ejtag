/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include <ch32v20x_usb.h>
#include <ch32v20x_rcc.h>
#include <ch32v20x_gpio.h>
#include "lsejtag_port.h"
#include "lsejtag/lsejtag_impl.h"
#include "hwdef.h"
#include "debug.h"
#include "usb_descriptors.h"

/* Global typedef */

/* Global define */

/* Global Variable */
lsejtag_ctx lsejtag_lib_ctx;
ejtag_ctx_t ejtag_ctx;
volatile uint32_t systick_counter;

/* Global function declaration */
void init_hardware();
void init_software();
void init_reset_tap_via_tms();

void led_task();

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    init_hardware();
    init_software();
    init_reset_tap_via_tms();

    while(1) {
        led_task();

        switch (lsejtag_dispatch(&lsejtag_lib_ctx)) {
        case dpr_execute:
            lsejtag_execute(&lsejtag_lib_ctx);
            break;
        case dpr_incomplete:
            break;
        case dpr_flush_tdo:
            lsejtag_flush_tdo(&lsejtag_lib_ctx);
            break;
        case dpr_run_jtag:
            lsejtag_run_jtag(&lsejtag_lib_ctx);
            break;
        case dpr_too_long:
            printf("PANIC: LSEJTAG command too long");
            while (1);
        case dpr_unknown_cmd:
            break;
        case dpr_corrupt_state:
            printf("!!!!!! dpr_corrput_state !!!!!!\n");
            printf("cmd buf dump:\n");
            // dump_binary_to_console(ctx->buffered_cmd, sizeof(ctx->buffered_cmd));
            printf("buffered_length: %d\r", (int)lsejtag_lib_ctx.buffered_length);
            printf("wait_payload_size: %d\n", (int)lsejtag_lib_ctx.cmd_buf_wait_payload_size);
            printf("state: %d\n",lsejtag_lib_ctx.cmd_buf_state);
            printf("PANIC: LSEJTAG corrupt state");
            while (1);
        case dpr_clean:
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//          Tasks
////////////////////////////////////////////////////////////////////////////////

void led_task() {
    // LED illumination logic
    if (ejtag_ctx.led_turn_on) {
        // Start timing and turn on
        ejtag_ctx.led_turn_on = 0;
        ejtag_ctx.led_timer_ms = systick_counter;
        lsejtag_impl_io_manip(impl_io_led, 1);
        return;
    }

    if (ejtag_ctx.led_timer_ms == 0) {
        return;
    }

    // 300ms
    if (systick_counter - ejtag_ctx.led_timer_ms >= 300) {
        // TURN OFF
        lsejtag_impl_io_manip(impl_io_led, 0);
        ejtag_ctx.led_timer_ms = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
//          Hardware Initialization code
////////////////////////////////////////////////////////////////////////////////

void init_sys() {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    // printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    // printf("This is printf example\r\n");
}

void init_gpio() {
    // Status & Activity Light
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD0PD1, ENABLE);
    GPIO_InitTypeDef gpioInit;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = EJTAG_LED_ACTIVITY_PIN;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(EJTAG_LED_ACTIVITY_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_LED_STATUS_PIN;
    GPIO_Init(EJTAG_LED_STATUS_GPIO, &gpioInit);

    // Illuminate status LED
    GPIO_ResetBits(EJTAG_LED_STATUS_GPIO, EJTAG_LED_STATUS_PIN);
    GPIO_SetBits(EJTAG_LED_ACTIVITY_GPIO, EJTAG_LED_ACTIVITY_PIN);

    // Output group: TRST, TDI, TMS, TCK, BRST, DINT
    EJTAG_GPIO_CLK_ENABLE;
    gpioInit.GPIO_Pin = EJTAG_TRST_PIN;
    GPIO_Init(EJTAG_TRST_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_TDI_PIN;
    GPIO_Init(EJTAG_TDI_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_TMS_PIN;
    GPIO_Init(EJTAG_TMS_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_TCK_PIN;
    GPIO_Init(EJTAG_TCK_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_BRST_PIN;
    GPIO_Init(EJTAG_BRST_GPIO, &gpioInit);
    gpioInit.GPIO_Pin = EJTAG_DINT_PIN;
    GPIO_Init(EJTAG_DINT_GPIO, &gpioInit);

    // Pull reset pins high
    GPIO_SetBits(EJTAG_TRST_GPIO, EJTAG_TRST_PIN);
    GPIO_SetBits(EJTAG_BRST_GPIO, EJTAG_BRST_PIN);
    
    // Put other pints low
    GPIO_ResetBits(EJTAG_TDI_GPIO, EJTAG_TDI_PIN);
    GPIO_ResetBits(EJTAG_TMS_GPIO, EJTAG_TMS_PIN);
    GPIO_ResetBits(EJTAG_TCK_GPIO, EJTAG_TCK_PIN);
    GPIO_ResetBits(EJTAG_DINT_GPIO, EJTAG_DINT_PIN);

    // Input group: TDO
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Pin = EJTAG_TDO_PIN;
    GPIO_Init(EJTAG_TDO_GPIO, &gpioInit);

    // Analog input: VIO (Not used)
    // TODO: do not output any signal unless voltage on VIO rises up to 90% 3.3V
    // gpioInit.GPIO_Mode = GPIO_Mode_AIN;
    // gpioInit.GPIO_Pin = EJTAG_VIO_PIN;
    // GPIO_Init(EJTAG_VIO_GPIO, &gpioInit);
}

void init_hardware() {
    init_sys();
    init_gpio();
}

/**
 * @brief USB device initialization callback for CherryUSB.
 */
void usb_dc_low_level_init(void) {
    // Configure GPIO A11/A12 as push-pull, low level to prevent spurious plug-in event on PC
    // as suggested on reference manual
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpioInit;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11 | GPIO_Pin_12);

    // Enable DP/DM internal pull-up
    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;

    // Select USB 48MHz clock: System core clock is 144MHz, 144/3=48MHz
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div3);
    
    // Enable USBD clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

    Delay_Ms(1);
}

////////////////////////////////////////////////////////////////////////////////
//          Software Initialization code
////////////////////////////////////////////////////////////////////////////////

void init_lsejtag() {
    lsejtag_init_ctx(&lsejtag_lib_ctx);

    ejtag_ctx.led_turn_on = false;
}

void init_usbd_interrupt() {
    // Enable IRQ should be done after CherryUSB has initialized FSDev
    // Interrupts flags are clear, we're safe to go right now
    NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

    // USBD wakeup EXTI line
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 	 

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void init_software() {
    init_cherryusb(0, 0x40005C00);
    init_usbd_interrupt();
    init_lsejtag();
}

void init_reset_tap_via_tms() {
    void lsejtag_impl_run_jtag(const uint32_t *tdi_buf, const uint32_t *tms_buf, uint32_t *tdo_buf,
                           uint32_t tdi_bits, uint32_t tdo_bits, uint32_t tdo_skip_bits);
    const uint32_t tdi = 0, tms = 0x0FFFFFFF;
    lsejtag_impl_run_jtag(&tdi, &tms, NULL, 32, 0, 0);
}
