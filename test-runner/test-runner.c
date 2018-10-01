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
#include <trusty/rpmb.h>
#include <trusty/trusty_dev.h>
#include <trusty/trusty_ipc.h>

enum test_message_header {
    TEST_PASSED = 0,
    TEST_FAILED = 1,
    TEST_MESSAGE = 2,
};

bool starts_with(const char* str1, const char* str2, size_t str2_len) {
    for (size_t i = 0; i < str2_len; i++) {
        if (str1[i] == '\0') {
            return true;
        }
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return false;
}

static void write_str(int fd, const char* str) {
    host_write(fd, str, trusty_strlen(str));
}

/*
 * Any return from this function indicates an internal error. The caller is
 * responsible for reporting the error. It currently returns to the host with
 * 2 as the exit code.
 */
void boot(int cpu) {
    int ret;
    static int stdout;
    int chan;
    int status;
    char cmdline[256];
    size_t cmdline_len;
    const char* port;
    const char boottest_cmd[] = "boottest ";
    char test_result[256];
    struct trusty_ipc_iovec iovec = {
            .base = test_result,
            .len = sizeof(test_result),
    };
    static struct trusty_dev trusty_dev;
    struct trusty_ipc_dev* ipc_dev;
    struct trusty_ipc_chan test_chan;

    if (cpu) {
        while (true) {
            ret = trusty_dev_nop(&trusty_dev);
            if (!ret) {
                trusty_idle(&trusty_dev, false);
            } else {
                write_str(stdout, "Secondary cpu unexpected error code\n");
            }
        }
    }

    /* Read test arguments from host (port name of test server to connect to) */
    cmdline_len = host_get_cmdline(cmdline, sizeof(cmdline));
    if (!starts_with(boottest_cmd, cmdline, cmdline_len)) {
        /* No test was requested, boot next operating system */
        boot_next();
        return;
    }
    port = cmdline + sizeof(boottest_cmd) - 1;

    /* Open special file ":tt" for write to select stdout */
    stdout = host_open(":tt", HOST_OPEN_MODE_W);

    /* Init Trusty device */
    ret = trusty_dev_init(&trusty_dev, NULL);
    if (ret != 0) {
        return;
    }

    /* Create Trusty IPC device */
    ret = trusty_ipc_dev_create(&ipc_dev, &trusty_dev, PAGE_SIZE);
    if (ret != 0) {
        return;
    }

    ret = rpmb_storage_proxy_init(ipc_dev, NULL);
    if (ret != 0) {
        write_str(stdout, "Failed to initialize storage proxy\n");
        return;
    }

    ret = arch_start_secondary_cpus();
    if (ret) {
        write_str(stdout, "Failed to start secondary CPUs\n");
        return;
    }

    /* Create connection to test server */
    trusty_ipc_chan_init(&test_chan, ipc_dev);
    chan = trusty_ipc_connect(&test_chan, port, true);
    if (chan < 0) {
        write_str(stdout, "Failed to connect to test server\n");
        return;
    }

    /* Wait for tests to complete and read status */
    while (true) {
        ret = trusty_ipc_recv(&test_chan, &iovec, 1, /* wait = */ true);
        if (ret <= 0 || ret >= (int)sizeof(test_result)) {
            return;
        }

        if (test_result[0] == TEST_PASSED) {
            break;
        } else if (test_result[0] == TEST_FAILED) {
            break;
        } else if (test_result[0] == TEST_MESSAGE) {
            host_write(stdout, test_result + 1, ret - 1);
        } else {
            return;
        }
    }
    status = test_result[0] != TEST_PASSED;

    /* Request another read to wait for the sever to close the connection */
    ret = trusty_ipc_recv(&test_chan, &iovec, 1, /* wait = */ true);
    if (ret != TRUSTY_ERR_CHANNEL_CLOSED) {
        return;
    }

    /* Return test status to host, 0: test success, 1: test failed */
    host_exit(status);
}
