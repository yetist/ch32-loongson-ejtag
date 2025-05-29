// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <stdint.h>
#include "chry_ringbuffer.h"

enum UsbDescItf
{
  ITF_NUM_EJTAG,
//   ITF_NUM_CDC,
//   ITF_NUM_CDC_DATA,
  ITF_NUM_TOTAL
};

enum UsbDescEp
{
  EPNUM_EJTAG_OUT = 2,
//   EPNUM_CDC_NOTIF = 3,
//   EPNUM_CDC_OUT   = 4,
//   EPNUM_CDC_IN    = 5,
  EPNUM_EJTAG_IN  = 6,
};

extern chry_ringbuffer_t rb_ejtag_read;
extern chry_ringbuffer_t rb_ejtag_write;
extern volatile bool ep_tx_busy_flag;
extern volatile bool ep_rx_busy_flag;

void init_cherryusb(uint8_t busid, uint32_t reg_base);
