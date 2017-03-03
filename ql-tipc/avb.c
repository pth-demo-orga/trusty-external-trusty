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

#include <trusty/avb.h>
#include <trusty/rpmb.h>
#include <trusty/trusty_ipc.h>
#include <trusty/util.h>

#define LOCAL_LOG 0

static bool initialized;
static int avb_tipc_version = 1;
static struct trusty_ipc_chan avb_chan;

static int avb_send_request(struct avb_message *msg, void *req, size_t req_len)
{
    struct trusty_ipc_iovec req_iovs[2] = {
        { .base = msg, .len = sizeof(*msg) },
        { .base = req, .len = req_len },
    };

    return trusty_ipc_send(&avb_chan, req_iovs, req ? 2 : 1, true);
}

static int avb_read_response(struct avb_message *msg, uint32_t cmd, void *resp,
                             size_t resp_len)
{
    int rc;
    struct trusty_ipc_iovec resp_iovs[2] = {
        { .base = msg, .len = sizeof(*msg) },
        { .base = resp, .len = resp_len },
    };

    rc = trusty_ipc_recv(&avb_chan, resp_iovs, resp ? 2 : 1, true);
    if (rc < 0) {
        trusty_error("failed (%d) to recv response\n", rc);
        return rc;
    }
    if (msg->cmd != (cmd | AVB_RESP_BIT)) {
        trusty_error("malformed response\n");
        return TRUSTY_ERR_GENERIC;
    }
    return rc;
}

static int avb_do_tipc(uint32_t cmd, void *req, uint32_t req_size, void *resp,
                       uint32_t resp_size, bool handle_rpmb)
{
    int rc;
    struct avb_message msg = { .cmd = cmd };

    if (!initialized && cmd != AVB_GET_VERSION) {
        trusty_error("%s: AVB TIPC client not initialized\n", __func__);
        return TRUSTY_ERR_GENERIC;
    }

    rc = avb_send_request(&msg, req, req_size);
    if (rc < 0) {
        trusty_error("%s: failed (%d) to send AVB request\n", __func__, rc);
        return rc;
    }

    if (handle_rpmb) {
        /* handle any incoming RPMB requests */
        rc = rpmb_storage_proxy_poll();
        if (rc < 0) {
            trusty_error("%s: failed (%d) to get RPMB requests\n", __func__,
                         rc);
            return rc;
        }
    }

    rc = avb_read_response(&msg, cmd, resp, resp_size);
    if (rc < 0) {
        trusty_error("%s: failed (%d) to read AVB response\n", __func__, rc);
        return rc;
    }

    return msg.result;
}

static int avb_get_version(uint32_t *version)
{
    int rc;
    struct avb_get_version_resp resp;

    rc = avb_do_tipc(AVB_GET_VERSION, NULL, 0, &resp, sizeof(resp), false);

    *version = resp.version;
    return rc;
}


int avb_tipc_init(struct trusty_ipc_dev *dev)
{
    int rc;
    uint32_t version = 0;

    trusty_assert(dev);
    trusty_assert(!initialized);

    trusty_ipc_chan_init(&avb_chan, dev);
    trusty_debug("Connecting to AVB service\n");

    /* connect to AVB service and wait for connect to complete */
    rc = trusty_ipc_connect(&avb_chan, AVB_PORT, true);
    if (rc < 0) {
        trusty_error("failed (%d) to connect to '%s'\n", rc, AVB_PORT);
        return rc;
    }

    /* check for version mismatch */
    rc = avb_get_version(&version);
    if (rc != 0) {
        trusty_error("Error getting version");
        return TRUSTY_ERR_GENERIC;
    }
    if (version != avb_tipc_version) {
        trusty_error("AVB TIPC version mismatch. Expected %u, received %u\n",
                     avb_tipc_version, version);
        return TRUSTY_ERR_GENERIC;
    }

    /* mark as initialized */
    initialized = true;

    return TRUSTY_ERR_NONE;
}

void avb_tipc_shutdown(struct trusty_ipc_dev *dev)
{
    if (!initialized)
        return; /* nothing to do */

    /* close channel */
    trusty_ipc_close(&avb_chan);

    initialized = false;
}

int trusty_read_rollback_index(uint32_t slot, uint64_t *value)
{
    int rc;
    struct avb_rollback_req req = { .slot = slot, .value = 0 };
    struct avb_rollback_resp resp;

    rc = avb_do_tipc(READ_ROLLBACK_INDEX, &req, sizeof(req), &resp,
                     sizeof(resp), true);

    *value = resp.value;
    return rc;
}

int trusty_write_rollback_index(uint32_t slot, uint64_t value)
{
    int rc;
    struct avb_rollback_req req = { .slot = slot, .value = value };
    struct avb_rollback_resp resp;

    rc = avb_do_tipc(WRITE_ROLLBACK_INDEX, &req, sizeof(req), &resp,
                     sizeof(resp), true);
    return rc;
}
