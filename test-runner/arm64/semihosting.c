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

#include <test-runner-arch.h>

#include <stddef.h>
#include <stdint.h>
#include <trusty/sysdeps.h>

/*
 * Prototype for assembly function that trigger a semihosting call as defined
 * in the "Semihosting for AArch32 and AArch64 Release 2.0" document from ARM.
 */
uint64_t semihosting(uint32_t op, uint64_t param);

#define SYS_GET_CMDLINE (0x15)

#define SYS_EXIT (0x18)
#define ADP_Stopped_ApplicationExit (0x20026)

#define SYS_OPEN (0x01)

#define SYS_WRITE (0x05)

void host_exit(uint32_t code)
{
    uint64_t params[2] = {
        ADP_Stopped_ApplicationExit,
        code,
    };
    semihosting(SYS_EXIT, (uint64_t)params);
}

size_t host_get_cmdline(char *strbuf, size_t strbufsize)
{
    int ret;
    uint64_t params[2] = {
        (uint64_t)strbuf,
        strbufsize,
    };
    ret = semihosting(SYS_GET_CMDLINE, (uint64_t)params);
    if (ret)
        return 0;
    return params[1];
}

int host_open(const char *path, uint32_t mode)
{
    uint64_t params[3] = {
        (uint64_t)path,
        mode,
        trusty_strlen(path),
    };
    return semihosting(SYS_OPEN, (uint64_t)params);
}

int host_write(int handle, const void *data, size_t size)
{
    uint64_t params[3] = {
        handle,
        (uint64_t)data,
        size,
    };
    return semihosting(SYS_WRITE, (uint64_t)params);
}
