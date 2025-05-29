// SPDX-License-Identifier: GPL-3.0-only

#pragma once

/*******************************************************************************
 *
 *                         N  O  T  I  C  E
 *
 *******************************************************************************
 *
 * Please define the hardware revision in the project properties from MounRiver
 * Studio. Add a preprocessor definition like:
 *
 * CH32LSEJTAG_HARDWARE_VERSION=0x0100
 *
 * and rebuild project.
 *
 ******************************************************************************/

#define HARDWARE_VERSION_V1_0 0x0100
#define HARDWARE_VERSION_V1_1 0x0101

#if (CH32LSEJTAG_HARDWARE_VERSION == HARDWARE_VERSION_V1_0)

#define EJTAG_GPIO_CLK_ENABLE                                                  \
    do {                                                                       \
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                  \
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                  \
    } while (0)

#define EJTAG_TRST_GPIO GPIOB
#define EJTAG_TDI_GPIO GPIOA
#define EJTAG_TDO_GPIO GPIOA
#define EJTAG_TMS_GPIO GPIOA
#define EJTAG_TCK_GPIO GPIOA
#define EJTAG_BRST_GPIO GPIOA
#define EJTAG_DINT_GPIO GPIOA
#define EJTAG_VIO_GPIO GPIOA

#define EJTAG_TRST_PIN GPIO_Pin_1
#define EJTAG_TDI_PIN GPIO_Pin_7
#define EJTAG_TDO_PIN GPIO_Pin_6
#define EJTAG_TMS_PIN GPIO_Pin_4
#define EJTAG_TCK_PIN GPIO_Pin_3
#define EJTAG_BRST_PIN GPIO_Pin_2
#define EJTAG_DINT_PIN GPIO_Pin_1
#define EJTAG_VIO_PIN GPIO_Pin_0

#define EJTAG_LED_STATUS_GPIO GPIOD
#define EJTAG_LED_ACTIVITY_GPIO GPIOD

#define EJTAG_LED_STATUS_PIN GPIO_Pin_0
#define EJTAG_LED_ACTIVITY_PIN GPIO_Pin_1

#elif (CH32LSEJTAG_HARDWARE_VERSION == HARDWARE_VERSION_V1_1)

#define EJTAG_GPIO_CLK_ENABLE                                                  \
    do {                                                                       \
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                  \
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                  \
    } while (0)

#define EJTAG_TRST_GPIO GPIOB
#define EJTAG_TDI_GPIO GPIOA
#define EJTAG_TDO_GPIO GPIOA
#define EJTAG_TMS_GPIO GPIOA
#define EJTAG_TCK_GPIO GPIOA
#define EJTAG_BRST_GPIO GPIOA
#define EJTAG_DINT_GPIO GPIOA
#define EJTAG_VIO_GPIO GPIOA

#define EJTAG_TRST_PIN GPIO_Pin_1
#define EJTAG_TDI_PIN GPIO_Pin_7
#define EJTAG_TDO_PIN GPIO_Pin_6
#define EJTAG_TMS_PIN GPIO_Pin_4
#define EJTAG_TCK_PIN GPIO_Pin_3
#define EJTAG_BRST_PIN GPIO_Pin_5
#define EJTAG_DINT_PIN GPIO_Pin_1
#define EJTAG_VIO_PIN GPIO_Pin_0

#define EJTAG_LED_STATUS_GPIO GPIOD
#define EJTAG_LED_ACTIVITY_GPIO GPIOD

#define EJTAG_LED_STATUS_PIN GPIO_Pin_0
#define EJTAG_LED_ACTIVITY_PIN GPIO_Pin_1

#endif
