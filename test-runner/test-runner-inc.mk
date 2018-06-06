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

TEST_RUNNER_DIR := $(GET_LOCAL_DIR)

TEST_RUNNER_MEMBASE ?= 0x60000000 # Default MEMBASE for arm64 qemu

XBIN_NAME := test-runner
XBIN_TOP_MODULE := $(TEST_RUNNER_DIR)
XBIN_ARCH := $(ARCH)
XBIN_BUILDDIR := $(BUILDDIR)/test-runner
XBIN_LINKER_SCRIPT := $(BUILDDIR)/test-runner.ld

# rules for generating the linker script
$(BUILDDIR)/test-runner.ld: $(TEST_RUNNER_DIR)/$(XBIN_ARCH)/test-runner.ld linkerscript.phony
	@echo generating $@
	@$(MKDIR)
	$(NOECHO)sed "s/%MEMBASE%/$(TEST_RUNNER_MEMBASE)/g" < $< > $@.tmp
	@$(call TESTANDREPLACEFILE,$@.tmp,$@)

include make/xbin.mk
