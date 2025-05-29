// SPDX-License-Identifier: GPL-3.0-only

#include <stdlib.h>
#include <assert.h>
#include <ch32v20x_gpio.h>
#include "usbd_core.h"
#include "usb_descriptors.h"
#include "lsejtag_impl.h"
#include "lsejtag/lsejtag_impl.h"
#include "hwdef.h"

#define NOP asm volatile ("nop")
#define ASSERT_TDI EJTAG_TDI_GPIO->BSHR = EJTAG_TDI_PIN
#define DEASSERT_TDI EJTAG_TDI_GPIO->BSHR = (EJTAG_TDI_PIN << 16)
#define ASSERT_TMS EJTAG_TMS_GPIO->BSHR = EJTAG_TMS_PIN
#define DEASSERT_TMS EJTAG_TMS_GPIO->BSHR = (EJTAG_TMS_PIN << 16)
#define ASSERT_TCK EJTAG_TCK_GPIO->BSHR = EJTAG_TCK_PIN
#define DEASSERT_TCK EJTAG_TCK_GPIO->BSHR = (EJTAG_TCK_PIN << 16)
#define ACTIVITY_LED_ON EJTAG_LED_ACTIVITY_GPIO->BSHR = (EJTAG_LED_ACTIVITY_PIN << 16)
#define ACTIVITY_LED_OFF EJTAG_LED_ACTIVITY_GPIO->BSHR = EJTAG_LED_ACTIVITY_PIN
#define CAPTURE_TDO ((EJTAG_TDO_GPIO->INDR & (EJTAG_TDO_PIN)) ? 0x80000000 : 0)


// #define ASSERT_TDO EJTAG_TDO_GPIO->BSHR = EJTAG_DINT_PIN
// #define DEASSERT_TDO EJTAG_TDO_GPIO->BSHR = (EJTAG_DINT_PIN << 16)

extern lsejtag_ctx lsejtag_lib_ctx;

void dump_binary_to_console(const uint8_t * const data, uint32_t length)
{
// #ifndef DISABLE_DBGPRINT
  char HexBuf[54] = {' ', '>', 0};
  uint32_t BytesLeft = length, Offset = 0;

  while (BytesLeft)
  {
    /* Print in 16 byte groups */
    uint32_t BytesToPrint = (BytesLeft > 16) ? 16 : BytesLeft;
    HexBuf[2] = '\0';
    for (int i = 0; i < BytesToPrint; i++)
    {
      char Tmp[3];
      sprintf(Tmp, "%02X", data[Offset + i]);
      strcat(HexBuf, Tmp);
      if (i == 7 && length != 7)
        strcat(HexBuf, "-");
      else
        strcat(HexBuf, " ");
    }
    // strcat(HexBuf, "\r\n");
    puts(HexBuf);

    BytesLeft -= BytesToPrint;
    Offset += BytesToPrint;
    HexBuf[0] = ' ';
  }
// #endif
}

uint32_t lsejtag_impl_usbrx_len() {
    return chry_ringbuffer_get_used(&rb_ejtag_read);
}

uint32_t lsejtag_impl_usbtx_free_len() {
    return chry_ringbuffer_get_free(&rb_ejtag_write);
}

void lsejtag_impl_usbrx_consume(uint8_t *dest, uint32_t len) {
    // uint32_t actual = 
    chry_ringbuffer_read(&rb_ejtag_read, dest, len);

    // printf("Expected %d, Consumed %d:", len, actual);
    // dump_binary_to_console(dest, len);

    // If the USB RX side has stalled and waiting for us to reinitiate the read, we do it
    if (!ep_rx_busy_flag && chry_ringbuffer_check_empty(&rb_ejtag_read)) {
        // Reset heads to the start
        chry_ringbuffer_reset(&rb_ejtag_read);

        // Fetch the write pointer and linear read size
        uint32_t avail_len;
        void* dest_ptr = chry_ringbuffer_linear_write_setup(&rb_ejtag_read, &avail_len);

        // Setup initial USB transfer
        ep_rx_busy_flag = true;
        usbd_ep_start_read(0, USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT), dest_ptr,
                           usbd_get_ep_mps(0, USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT)));
    }
}

void lsejtag_impl_usbtx(const uint8_t *data, uint32_t len) {
    chry_ringbuffer_write(&rb_ejtag_write, (void*)data, len);

    // Start the initial TX action
    if (!ep_tx_busy_flag) {
        uint32_t avail_len;
        void *read_addr = chry_ringbuffer_linear_read_setup(&rb_ejtag_write, &avail_len);
        avail_len = MIN(usbd_get_ep_mps(0, USB_ENDPOINT_IN(EPNUM_EJTAG_IN)), avail_len);
        usbd_ep_start_write(0, USB_ENDPOINT_IN(EPNUM_EJTAG_IN), read_addr, avail_len);
        ep_tx_busy_flag = true;
    }
}

void lsejtag_impl_io_manip(lsejtag_impl_io io, uint8_t level) {
    switch (io) {
    case impl_io_led:

        break;
    case impl_io_trst:
        break;
    case impl_io_brst:
        break;
    case impl_io_dint:
        break;
    default:
        break;
    }
}

void lsejtag_impl_reconfigure(lsejtag_impl_recfg type, uint32_t param) {
    switch (type) {
    case impl_recfg_clkfreq: {
        // FIXME: Reconfigure
        break;
    }

    case impl_recfg_tdo_sample_time:
        break;
    }
}

#define HUGE_DELAY //Delay_Us(20)

static inline char BufId() {
    if (lsejtag_lib_ctx.active_bufblk == &lsejtag_lib_ctx.jtagbuf_a) {
        return 'A';
    } else if (lsejtag_lib_ctx.active_bufblk == &lsejtag_lib_ctx.jtagbuf_b) {
        return 'B';
    }
    return '?';
}

#pragma GCC push_options
#pragma GCC optimize("O2")
void lsejtag_impl_run_jtag(const uint32_t *tdi_buf, const uint32_t *tms_buf, uint32_t *tdo_buf,
                           uint32_t tdi_bits, uint32_t tdo_bits, uint32_t tdo_skip_bits) {
    //

    uint32_t tdi_reg, tms_reg, tdo_reg = 0;
    uint32_t tdo_counter = 0;

    ACTIVITY_LED_ON;

    // printf("RUN_JTAG: Buf%c, TDI/TMS Len % 2d, TDO Len % 2d, TDO Skip % 2d\r\n",
    //        BufId(), tdi_bits, tdo_bits, tdo_skip_bits);

    // Make this variable exactly 0 when the tdo_skip_bits-th CLK negedge arrives
    // --tdo_skip_bits;

    // A smaller inner loop for each DWORD of data
    const uint32_t out_dwords = ((tdi_bits + 31) >> 5);
    for (uint32_t i = 0; i < out_dwords; ++i) {
        tdi_reg = *tdi_buf;
        tms_reg = *tms_buf;
        ++tdi_buf;
        ++tms_buf;
        for (uint32_t bit = (tdi_bits > 32 ? 32 : tdi_bits); bit > 0; --bit) {
            // Setup data bit
            if (tdi_reg & 1) {
                ASSERT_TDI;
            } else {
                DEASSERT_TDI;
            }
            if (tms_reg & 1) {
                ASSERT_TMS;
            } else {
                DEASSERT_TMS;
            }
            
            // Wait a bit
            tdi_reg >>= 1;
            tms_reg >>= 1;

            // Assert CLK
            HUGE_DELAY;
            ASSERT_TCK;
            // Check if TDO should be captured
            if (tdo_skip_bits) {
                --tdo_skip_bits;
                NOP;
                NOP;
                NOP;
                NOP;
                NOP;
                NOP;
                NOP;
            } else if (tdo_counter < tdo_bits) {
                ++tdo_counter;

                // Capture TDO
                tdo_reg >>= 1;
                tdo_reg |= CAPTURE_TDO;

                // ASSERT_TDO;
                // NOP;
                // NOP;
                // NOP;
                // NOP;
                // DEASSERT_TDO;
            }

            HUGE_DELAY;
            HUGE_DELAY;
            
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;

            // Deassert CLK
            DEASSERT_TCK;

            // Store the captured TDO bits
            if (tdo_counter && tdo_counter % 32 == 0) {
                *tdo_buf = tdo_reg;
                ++tdo_buf;
            }
            
            HUGE_DELAY;
        }
        tdi_bits -= 32;
    }

    // Account for the last DWORD of TDO data
    if (tdo_counter && (tdo_counter % 32 != 0)) {
        *tdo_buf = tdo_reg;
    }

    // Finish
    lsejtag_jtag_complete_tdi(&lsejtag_lib_ctx);
    lsejtag_jtag_complete_tms(&lsejtag_lib_ctx);
    lsejtag_jtag_complete_tdo(&lsejtag_lib_ctx, ((tdo_counter + 31) >> 5));

    ACTIVITY_LED_OFF;
}
#pragma GCC pop_options
