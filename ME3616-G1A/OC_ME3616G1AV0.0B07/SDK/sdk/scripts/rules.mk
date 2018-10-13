################################################################################
##
## File   : rules.mk
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
#SDK_PATH := $(realpath ../sdk)
ifeq ($(SDK_PATH),)
SDK_PATH := $(realpath ../sdk)
$(warnning SDK_PATH not set, default to "$(SDK_PATH)")
endif

################################################################################
# target
################################################################################
#TARGET := hello
ifeq ($(TARGET),)
TARGET := app
endif

################################################################################
# common variables
################################################################################
#DEBUG := 1
#OPT := -Og -g3

################################################################################
# source
################################################################################
#C_SOURCES := \
#	hello.c \
#	syscalls.c \

#CXX_SOURCES := \

#S_SOURCES := \
#	head.s

ifeq ($(LDSCRIPT),)
ifeq ($(TARGET_TYPE),RAM_APP)
LDSCRIPT := $(SDK_PATH)/scripts/oem_ram_app.ld
else
LDSCRIPT := $(SDK_PATH)/scripts/oem_app.ld
endif
endif

ifeq ($(DL_CONFIG),)
DL_CONFIG := $(SDK_PATH)/scripts/download.cfg
endif

################################################################################
# CFLAGS
################################################################################
#C_DEFS := \

#C_INCLUDES := \

################################################################################
# ASFLAGS
################################################################################
# macros for gcc
#AS_DEFS := \

# includes for gcc
#AS_INCLUDES := \

################################################################################
# LDFLAGS
################################################################################
# libraries
#LD_LIBS := \

################################################################################
# defaut target
################################################################################
ifeq ($(TARGET),)
TARGET := app
endif

################################################################################
# building variables
################################################################################
ifeq ($(V),)
V := 0
endif

ifeq ($(V), 1)
Q :=
else
Q := @
endif

ifeq ($(BIN_CKSUM),)
BIN_CKSUM := 1
endif

################################################################################
# binaries
################################################################################
BB := busybox
BUSYBOX := $(Q)$(BB)
MKDIR := $(Q)$(BB) mkdir -p
RM := $(Q)$(BB) rm
CAT := $(Q)$(BB) cat
ECHO := @$(BB) echo -e

################################################################################
# Compiler binaries
################################################################################
ifeq ($(CROSS_PREFIX),)
CROSS_PREFIX := arm-none-eabi-
endif
CC := $(Q)$(CROSS_PREFIX)gcc
CXX := $(Q)$(CROSS_PREFIX)g++
AS := $(Q)$(CROSS_PREFIX)gcc -x assembler-with-cpp
LD := $(Q)$(CROSS_PREFIX)gcc
CP := $(Q)$(CROSS_PREFIX)objcopy
AR := $(Q)$(CROSS_PREFIX)ar
SZ := $(Q)$(CROSS_PREFIX)size
CPP := $(Q)$(CROSS_PREFIX)cpp

ifeq ($(CXX_SOURCES),)
LD := $(CC)
else
LD := $(CXX)
endif

################################################################################
# pathes
################################################################################
# Build path
ifeq ($(BIN_DIR),)
BIN_DIR := bin
endif

ifeq ($(OBJ_DIR),)
OBJ_DIR := obj
endif

################################################################################
# building variables
################################################################################
BUILD_OS_STIRNG := $(shell $(BB) uname)
ifneq ($(findstring Windows,$(BUILD_OS_STIRNG)),)
BUILD_OS := Windows
else ifneq ($(findstring Linux,$(BUILD_OS_STIRNG)),)
BUILD_OS := Linux
else ifneq ($(findstring MINGW,$(BUILD_OS_STIRNG)),)
BUILD_OS := MINGW
else
$(error Unsupported build OS: $(BUILD_OS_STIRNG))
endif

GCC_VERSION_MAJOR := $(shell $(subst @,,$(CC)) -dumpversion | $(BB) cut -d. -f1)
#$(warning GCC_VERSION_MAJOR: $(GCC_VERSION_MAJOR))

# optimization
ifeq ($(OPT),)
ifeq ($(DEBUG), 1)
OPT := -Og -g3
else
OPT := -Os -g
endif
endif

OPT += -pipe

CPU_FLAGS := \
	-mlittle-endian \
	-mcpu=cortex-m4 \
	-mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \

################################################################################
# CFLAGS
################################################################################
ifeq ($(DEBUG), 1)
C_DEFS += -DDEBUG_BUILD
endif

C_INCLUDES += \
	-I$(SDK_PATH)/include \
	-I$(SDK_PATH)/FreeRTOS/Source/include \
	-I$(SDK_PATH)/FreeRTOS/Source/portable/GCC/mt2625/ARM_CM4F \
	-I$(SDK_PATH)/lwip/src/include \
	-I$(SDK_PATH)/lwip/ports/include \

# Generate dependency information
# Generate list file
MISC_FLAGS = \
	-MD -MP -MF $(OBJ_DIR)/$(@F).d \
	-Wa,-a,-ad,-alms=$(OBJ_DIR)/$(@F).lst \

COMM_FLAGS = \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-fsingle-precision-constant \
	-fno-strict-aliasing \
	-Werror \
	-Wall \
	-Wextra \
	-Wunused \
	-Wuninitialized \
	-Wpointer-arith \
	-Wshadow \
	-Wlogical-op \
	-Waggregate-return \
	-Wfloat-equal \
	-Wno-unused-parameter \
	$(C_DEFS) \
	$(C_INCLUDES) \
	$(MISC_FLAGS) \

#	-Wconversion \
#	-Wno-sign-compare \
#	-Waggregate-return \
#	-Wmissing-declarations \
#	-Wconversion \
#	-Wpadded \
#	-Wuninitialized \
#	-fstack-usage \
#	-funwind-tables \
#	-Wstack-usage=256 \

CFLAGS = \
	$(OPT) \
	$(CPU_FLAGS) \
	-std=gnu11 \
	$(COMM_FLAGS) \

CXXFLAGS = \
	$(OPT) \
	$(CPU_FLAGS) \
	-std=gnu++11 \
	$(COMM_FLAGS) \
	-fabi-version=0 \
	-fno-exceptions \
	-fno-rtti \
	-fno-use-cxa-atexit \
	-fno-threadsafe-statics \
	-Wabi \
	-Wctor-dtor-privacy \
	-Wnoexcept \
	-Wnon-virtual-dtor \
	-Wstrict-null-sentinel \
	-Wsign-promo \
	
################################################################################
# ASFLAGS
################################################################################
ASFLAGS = \
	$(OPT) \
	$(CPU_FLAGS) \
	-ffunction-sections \
	-fdata-sections \
	-Wall \
	-Wextra \

# Generate list file
ASFLAGS += \
	-Wa,-a,-ad,-alms=$(OBJ_DIR)/$(@F).lst \

ASFLAGS += \
	$(AS_DEFS) \
	$(AS_INCLUDES) \

################################################################################
# LDFLAGS
################################################################################
LDFLAGS = \
	$(CPU_FLAGS) \
	-nostartfiles \
	--specs=nano.specs \

# generate map file
LDFLAGS += \
	-Wl,-Map=$(BIN_DIR)/$(basename $(@F)).map,--cref

# remonve ununsed sections
LDFLAGS += \
	-Wl,--gc-sections

ifneq ($(findstring $(GCC_VERSION_MAJOR),5 6 7 8),)
LDFLAGS += -Wl,--print-memory-usage
endif

LDFLAGS += \
	-L$(SDK_PATH)/lib \
	
# libraries
LD_LIBS += \

################################################################################
# Flags for objcopy
################################################################################
BIN_FLAGS := -O binary -S
HEX_FLAGS := -O ihex -S

################################################################################
# Target extension
################################################################################
ifeq ($(TARGET_TYPE), lib)
TARGET_EXT := a
else
TARGET_EXT := elf
endif

################################################################################
# myself
################################################################################
MAKE_FILE += $(MAKEFILE_LIST)
#$(warning $(MAKE_FILE))

################################################################################
# Defalt target
################################################################################
# default action: build all
ifeq ($(TARGET_TYPE), lib)
all: $(BIN_DIR)/$(TARGET).$(TARGET_EXT)
else
all: $(BIN_DIR)/$(TARGET).$(TARGET_EXT) $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/download.cfg
endif

$(BIN_DIR)/download.cfg: $(BIN_DIR)/$(TARGET).bin
	$(BUSYBOX) sh -c \
		"cat $(DL_CONFIG) | \
		sed -e 's/OEM_APP_NAME/$(TARGET)\.bin/g' > $@"

################################################################################
# build the application
################################################################################
# list of C objects
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.c.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of CXX objects
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.cpp.o)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(S_SOURCES:.s=.s.o)))
vpath %.s $(sort $(dir $(S_SOURCES)))

################################################################################
# Common rules
################################################################################
$(OBJ_DIR)/%.c.o: %.c $(MAKE_FILE) | $(OBJ_DIR)
	$(ECHO) "\tCC\t$(notdir $@)"
	$(CC) -c $(CFLAGS) $(CFLAGS_$(notdir $<)) $< -o $@

$(OBJ_DIR)/%.cpp.o: %.cpp $(MAKE_FILE) | $(OBJ_DIR)
	$(ECHO) "\tCC\t$(notdir $@)"
	$(CXX) -c $(CXXFLAGS) $(CFLAGS_$(notdir $<)) $< -o $@

$(OBJ_DIR)/%.s.o: %.s $(MAKE_FILE) | $(OBJ_DIR)
	$(ECHO) "\tAS\t$(notdir $@)"
	$(AS) -c $(ASFLAGS) $(ASFLAGS_$(notdir $<)) $< -o $@

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS) $(LDSCRIPT) | $(BIN_DIR)
	$(ECHO) "\tLN\t$(notdir $@)"
	$(LD) $(LDFLAGS) -T$(LDSCRIPT) $(OBJECTS) $(LD_LIBS) -o $@
	$(SZ) $@

$(BIN_DIR)/$(TARGET).a: $(OBJECTS) | $(BIN_DIR)
	$(ECHO) "\tAR\t$(notdir $@)"
	$(AR) -rcs $@ $(OBJECTS)

$(BIN_DIR)/%.hex: $(BIN_DIR)/%.elf
	$(ECHO) "\tHEX\t$(notdir $@)"
	$(CP) $(HEX_FLAGS) $< $@

$(BIN_DIR)/%.nocrc: $(BIN_DIR)/%.elf
	$(ECHO) "\tBIN\t$(notdir $@)"
	$(CP) $(BIN_FLAGS) $< $@

$(BIN_DIR)/%.bin: $(BIN_DIR)/%.nocrc
	$(ECHO) "\tCKSUM\t$(notdir $@)"
	$(CAT) $< > $@
ifeq ($(BUILD_OS),Windows)
	$(BUSYBOX) sh -c \
		"cksum $< | \
		xargs printf '%08X %d %s\n' | \
		awk '{printf(\"00000000 %s %s %s %s\n\", \
			substr($$1, 7, 2), substr($$1, 5, 2), substr($$1, 3, 2), substr($$1, 1, 2))}' | \
		$(BB) hexdump -R | \
		dd of=$@ obs=1 count=4 seek=4 conv=notrunc status=none"
else
	$(BUSYBOX) sh -c \
		"cksum $< | \
		xargs printf '%08X %d %s\n' | \
		awk '{printf(\"00000000 %s %s %s %s\n\", \
			substr(\$$1, 7, 2), substr(\$$1, 5, 2), substr(\$$1, 3, 2), substr(\$$1, 1, 2))}' | \
		$(BB) hexdump -R | \
		dd of=$@ obs=1 count=4 seek=4 conv=notrunc status=none"
endif

MAKE_DIRS += \
	$(OBJ_DIR) \
	$(BIN_DIR) \

define make-dir
$1:
	$(ECHO) "\tMKDIR\t$$(notdir $$@)"
	$(MKDIR) $$@
endef

$(foreach d,$(MAKE_DIRS),$(eval $(call make-dir,$d)))

################################################################################
# clean up
################################################################################
.PHONY : clean
clean:
	$(RM) -rf $(BIN_DIR) $(OBJ_DIR)

################################################################################
# dependencies
################################################################################
-include $(wildcard $(OBJ_DIR)/*.d)

# *** EOF ***

