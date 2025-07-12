// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <stdint.h>
#include <stdbool.h>

/*==============================================================================
 *                       P O R T I N G    G U I D E
 *==============================================================================
 *
 * 0. What you need to provide
 * 
 *  You need to provide USB TX (device to host, IN) and RX (host to device, OUT)
 *  FIFOs, IN endpoint should be 6 and OUT endpoint should be 2. You should
 *  write functions to provide USB FIFO status and read from/write into them.
 *  The library will assume data will come into RX FIFO and come out of TX FIFO
 *  automagically without library trying to operate with the hardware, so your
 *  TX and RX function will have to interface with the hardware.
 *
 *  Aside from that, you also need to provide TRST, BRST, DINT, Activity LED IO
 *  ports and implement a function to manipulate them. And most importantly, you
 *  need to implement a function to correctly execute the JTAG sequence that the
 *  library sent you.
 *
 * 1. Functions to implement
 *   
 *  uint32_t lsejtag_impl_usbrx_len();
 *      Should return how many bytes has been received from USB and can be taken
 *      from USB RX FIFO immediately by calling lsejtag_impl_usbrx_consume.
 *
 *  uint32_t lsejtag_impl_usbtx_free_len();
 *      Should return how many bytes can be put into USB TX FIFO immediately and
 *      sent out to host by calling lsejtag_impl_usbtx.
 *
 *  void lsejtag_impl_usbrx_consume(uint8_t *dest, uint32_t len);
 *      Should consume `len` bytes from your USB RX FIFO, and write them to
 *      buffer pointed to by `dest`. `len` will never be larger than the return
 *      value of `lsejtag_impl_usbrx_len()`, which will always be called before
 *      `lsejtag_impl_usbrx_consume()` was called.
 * 
 *  void lsejtag_impl_usbtx(uint8_t *data, uint32_t len);
 *      Should add `len` bytes into your USB TX FIFO. The sending part may be
 *      done later after `lsejtag_execute()` has returned. Each call to
 *      `lsejtag_execute()` will result in only one `lsejtag_impl_usbtx()` call.
 *      Sending of asynchronously collected TDO data is handled inside
 *      `lsejtag_flush_tdo()` when `lsejtag_dispatch()` asks you to do so.
 * 
 *  void lsejtag_impl_io_manip(lsejtag_impl_io io, uint8_t level);
 *      This function maps directly with the Activity LED, TRST, BRST, DINT IO
 *      ports. Whether you use open-collector or push-pull IO mode, `level`
 *      always must be the actual voltage level on the physical IO pin.
 *
 *  void lsejtag_impl_reconfigure(lsejtag_impl_recfg type, uint32_t param)
 *      Reconfiguration of JTAG hardware. Reconfiguration has two types:
 *      
 *      impl_recfg_clkfreq:
 *      Set JTAG clock frequency. The 32-bit parameter is interpreted as below:
 *      Find the highest set bit, and clear other bits. Divide 15MHz with the
 *      result. Now you get the frequency.
 *
 *      impl_recfg_tdo_sample_time:
 *      Set JTAG TDO sample time point. By standard, JTAG TDO should be sampled
 *      at TCK rising edge. Only the least significant 2 bits are checked:
 *      0 - half cycle before TCK rising edge (at previous falling edge)
 *      1 - standard (at TCK rising edge)
 *      2 - half cycle after TCK rising edge (at next falling edge)
 *      3 - one cycle after TCK rising edge (at next rising edge)
 *      This reconfiguration is not important. Most of the time you don't have
 *      to implement this.
 *
 *  void lsejtag_impl_run_jtag(const uint32_t *tdi_buf, const uint32_t *tms_buf,
 *                             uint32_t *tdo_buf, uint32_t tdi_bits,
 *                             uint32_t tdo_bits, uint32_t tdo_skip_bits)
 *      Should execute a JTAG sequence.
 *
 *      `tdi_buf` and `tms_buf` contains a series of 32-bit words. Bits should
 *      be shifted out of TDI/TMS pins, from LSB to MSB, up from array offset 0.
 *
 *      `tdo_buf` should be written into by your implementation. Bits should be
 *      captured from TDO pins, and filled into TDO buffer area, from LSB to MSB
 *      and up from array offset 0.
 *
 *      TDI and TMS sequence length should be of the same length (`tdi_bits`).
 *      TDO sequence captured should be of `tdo_bits` bits long.
 *      Your implementation should start capturing TDO bits after skipping
 *      `tdo_skip_bits` TCK rising edges (for example, skip 5 rising edges, then
 *      at the 6th rising edge, you begin capturing the first TDO bit)
 *
 *============================================================================*/

typedef enum {
    impl_io_led,
    impl_io_trst,
    impl_io_brst,
    impl_io_dint,
} lsejtag_impl_io;

typedef enum {
    impl_recfg_clkfreq,
    impl_recfg_tdo_sample_time,
} lsejtag_impl_recfg;

uint32_t lsejtag_impl_usbrx_len();

uint32_t lsejtag_impl_usbtx_free_len();

void lsejtag_impl_usbrx_consume(uint8_t *dest, uint32_t len);

void lsejtag_impl_usbtx(const uint8_t *data, uint32_t len);

void lsejtag_impl_io_manip(lsejtag_impl_io io, uint8_t level);

void lsejtag_impl_reconfigure(lsejtag_impl_recfg type, uint32_t param);

void lsejtag_impl_run_jtag(const uint32_t *tdi_buf, const uint32_t *tms_buf, uint32_t *tdo_buf,
                           uint32_t tdi_bits, uint32_t tdo_bits, uint32_t tdo_skip_bits);
