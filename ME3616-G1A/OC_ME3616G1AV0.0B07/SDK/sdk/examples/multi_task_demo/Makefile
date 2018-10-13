################################################################################
##
## File   : Makefile
##
## Copyright (C) 2013-2018 Gosuncn. All rights reserved.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## Author : lixingyuan@gosuncn.cn
##
##  $Date: 2018/02/08 08:45:36GMT+08:00 $
##
################################################################################

################################################################################
# sdk path
################################################################################
SDK_PATH := $(realpath ../..)

###############################################################################
# target
################################################################################
TARGET := multi_task_demo

################################################################################
# common variables
################################################################################
DEBUG := 1

################################################################################
# source
################################################################################
C_SOURCES := multi_task.c  \
		$(SDK_PATH)/src/gsdk_ril_cmds.c \
		$(SDK_PATH)/FreeRTOS/Source/portable/MemMang/heap_1.c

S_SOURCES :=

################################################################################
# CFLAGS
################################################################################
C_DEFS :=

C_INCLUDES :=

################################################################################
# ASFLAGS
################################################################################
# macros for gcc
AS_DEFS :=

# includes for gcc
AS_INCLUDES :=

################################################################################
# LDFLAGS
################################################################################
# libraries
LD_LIBS := -lgsdk

################################################################################
# Build rule
################################################################################
include $(SDK_PATH)/scripts/rules.mk

# *** EOF ***
