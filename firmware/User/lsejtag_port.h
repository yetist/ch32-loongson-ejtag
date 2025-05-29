// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "lsejtag/lsejtag.h"

typedef struct _ejtag_ctx_t {
    bool led_turn_on;
    uint32_t led_timer_ms;
} ejtag_ctx_t;

extern lsejtag_ctx lsejtag_lib_ctx;
extern ejtag_ctx_t ejtag_ctx;
