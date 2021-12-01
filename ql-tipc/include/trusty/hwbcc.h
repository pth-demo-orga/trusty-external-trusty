/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRUSTY_HWBCC_H
#define TRUSTY_HWBCC_H

#include <interface/hwbcc/hwbcc.h>
#include <trusty/trusty_ipc.h>

/*
 * Initialize HWBCC TIPC client. Returns one of trusty_err.
 *
 * @dev: trusty_ipc_dev
 */
int hwbcc_tipc_init(struct trusty_ipc_dev* dev);

/*
 * Shutdown HWBCC TIPC client.
 *
 * @dev: trusty_ipc_dev
 */
void hwbcc_tipc_shutdown(void);

/**
 * Retrieves DICE artifacts for a child node in the DICE chain/tree in
 * non-secure world (e.g. ABL).
 * @context:                    Context information passed in by the client.
 * @dice_artifacts:             Pointer to a buffer to store the CBOR encoded
 * DICE artifacts.
 * @dice_artifacts_buf_size:    Size of the buffer pointed by @dice_artifacts.
 * @dice_artifacts_size:        Actual size of the buffer used.
 */
int hwbcc_get_dice_artifacts(uint64_t context,
                             uint8_t* dice_artifacts,
                             size_t dice_artifacts_buf_size,
                             size_t* dice_artifacts_size);
/**
 * Deprivilege hwbcc from serving calls to non-secure clients.
 */
int hwbcc_ns_deprivilege(void);

#endif /*TRUSTY_HWBCC_H*/