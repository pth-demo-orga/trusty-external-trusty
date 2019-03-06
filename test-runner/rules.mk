#
# Copyright (c) 2018, Google, Inc. All rights reserved
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

QL_TIPC = $(LOCAL_DIR)/../ql-tipc

MODULE_DEFINES += \
	NS_ARCH_ARM64=1 \

MODULE_INCLUDES += \
	$(LOCAL_DIR)/include \
	$(LOCAL_DIR)/$(ARCH)/include \
	$(QL_TIPC)/include \
	$(LOCAL_DIR)/../interface/include \
	$(LOCAL_DIR)/../../lk/include \
	$(LOCAL_DIR)/../../lk/include/shared/lk \
	$(LOCAL_DIR)/../../lk/lib/libc/include \

MODULE_SRCS += \
	$(LOCAL_DIR)/test-runner.c \
	$(LOCAL_DIR)/test-runner-storage.c \
	$(LOCAL_DIR)/test-runner-sysdeps.c \
	$(LOCAL_DIR)/utils.c \
	$(LOCAL_DIR)/virtio.c \
	$(LOCAL_DIR)/virtio-console.c \
	$(QL_TIPC)/ipc.c \
	$(QL_TIPC)/ipc_dev.c \
	$(QL_TIPC)/rpmb_proxy.c \
	$(QL_TIPC)/util.c \
	$(QL_TIPC)/arch/arm/trusty_dev.c \

ifeq (true,$(call TOBOOL,$(VIRTIO_MMIO_DEVICE)))
MODULE_SRCS += $(LOCAL_DIR)/virtio-mmio.c
else
MODULE_SRCS += $(LOCAL_DIR)/virtio-pci.c
endif

include $(LOCAL_DIR)/$(ARCH)/rules.mk
include make/module.mk
