/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

.text

.globl _start
_start:
    /* Set exception vector base */
    adr x0, _start

    mrs x1, CurrentEL
    and x1, x1, #0xc
    cmp x1, #0x8
    bne vbar_setup_not_el2
    msr vbar_el2, x0
    b vbar_setup_done
vbar_setup_not_el2:
    msr vbar_el1, x0

vbar_setup_done:
    /* Setup stack */
    adr x0, stack
    mov sp, x0

    /* Jump to c-code */
    bl boot
    /* fall-through */

error:
    mov x0, #0x18
    adr x1, exit_params
    /* fall-through */

.globl semihosting
semihosting:
    hlt 0xf000
    ret

exit_params:
    .quad 0x20026 /* ADP_Stopped_ApplicationExit */
    .quad 2 /* exit code */

.globl trusty_idle
trusty_idle:
    wfi
    ret

/*
 * Catch exceptions (except sync-sp_el0 as we don't use that mode and it would
 * jump to our entry point) and exit in case there is a bug.
 */
.macro exception, addr
.org \addr
    b error
.endm

exception 0x080
exception 0x100
exception 0x180
exception 0x200
exception 0x280
exception 0x300
exception 0x380
exception 0x400
exception 0x480
exception 0x500
exception 0x580
exception 0x600
exception 0x680
exception 0x700
exception 0x780


.data
    .space 4096
stack:
