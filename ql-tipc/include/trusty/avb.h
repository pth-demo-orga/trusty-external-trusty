/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef TRUSTY_AVB_H_
#define TRUSTY_AVB_H_

#include <trusty/sysdeps.h>
#include <trusty/trusty_ipc.h>
#include <interface/avb/avb.h>

/*
 * Initialize AVB TIPC client. Returns one of trusty_err.
 *
 * @dev: initialized with trusty_ipc_dev_create
 */
int avb_tipc_init(struct trusty_ipc_dev *dev);
/*
 * Shutdown AVB TIPC client.
 *
 * @dev: initialized with trusty_ipc_dev_create
 */
void avb_tipc_shutdown(struct trusty_ipc_dev *dev);
/*
 * Send request to secure side to read rollback index.
 * Returns one of trusty_err.
 *
 * @slot:    rollback index slot
 * @value:   rollback index value stored here
 */
int trusty_read_rollback_index(uint32_t slot, uint64_t *value);
/*
 * Send request to secure side to write rollback index
 * Returns one of trusty_err.
 *
 * @slot:    rollback index slot
 * @value:   rollback index value to write
 */
int trusty_write_rollback_index(uint32_t slot, uint64_t value);

#endif /* TRUSTY_AVB_H_ */
