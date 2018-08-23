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

#include <trusty/rpmb.h>
#include <trusty/trusty_dev.h>
#include <trusty/util.h>

#include <test-runner-arch.h>

int rpmb_storage_send(void* rpmb_dev,
                      const void* rel_write_data,
                      size_t rel_write_size,
                      const void* write_data,
                      size_t write_size,
                      void* read_buf,
                      size_t read_size) {
    int ret;
    int fd;
    uint16_t read_count = read_size / MMC_BLOCK_SIZE;

    fd = host_open("RPMB_CMDRES", HOST_OPEN_MODE_WRB);
    if (fd < 0) {
        goto err_no_fd;
    }

    ret = host_write(fd, &read_count, sizeof(read_count));

    ret = host_write(fd, rel_write_data, rel_write_size);
    if (ret) {
        goto err;
    }
    ret = host_write(fd, write_data, write_size);
    if (ret) {
        goto err;
    }

    ret = host_system("./rpmb_dev --dev RPMB_DATA --cmd RPMB_CMDRES");
    if (ret) {
        goto err;
    }

    ret = host_read(fd, read_buf, read_size);
    if (ret) {
        goto err;
    }

    host_close(fd);

    return 0;

err:
    host_close(fd);
err_no_fd:
    return TRUSTY_ERR_GENERIC;
}
