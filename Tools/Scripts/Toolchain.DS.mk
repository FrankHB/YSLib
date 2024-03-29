#
#	(C) 2014-2016, 2021 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile for DS toolchain
# Version = r90
# Created = 2014-12-06 16:51:13 +0800
# Updated = 2021-12-12 02:52 +0800
# Encoding = ANSI


# NOTE: Workaround for libnds: do not use '-pedantic-errors'.
C_CXXFLAGS_COMMON ?= -pipe -fdata-sections -ffunction-sections \
	-fvisibility=hidden -DYB_Use_LightweightTypeID=1

# NOTE: Workaround for libnds: do not use '-Wsign-conversion'.
C_CXXFLAGS_WARNING ?= -Wall -Wcast-align -Wdeprecated \
	-Wdeprecated-declarations -Wextra -Wfloat-equal -Wformat=2 -Winvalid-pch \
	-Wmissing-declarations -Wmissing-include-dirs -Wmultichar \
	-Wno-format-nonliteral -Wredundant-decls -Wshadow
# NOTE: Workaround for libnds: do not use '-Wdouble-promotion'.
C_CXXFLAGS_IMPL_WARNING ?= -Wlogical-op -Wtrampolines

# NOTE: It is redundant to emit code for thread-safe initialization since the \
	implementation is with single thread model.
C_CXXFLAGS_COMMON += -fno-threadsafe-statics

# NOTE: Binary size is more important for DS. LTO is also disabled by default.
ifneq ($(CONF),debug)
	C_CXXFLAGS_OPT_LV ?= -Os
	CXXFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations
	LDFLAGS_IMPL_OPT ?= $(CXXFLAGS_IMPL_OPT)
else
	# NOTE: "-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC" is not used.
	C_CXXFLAGS_OPT_DBG ?= $(C_CXXFLAGS_OPT_LV) -g -fno-omit-frame-pointer
endif

include $(dir $(lastword $(MAKEFILE_LIST)))Toolchain.options.mk

# NOTE: Wordaround for client makefiles.
CFLAGS_WARNING := $(CFLAGS_WARNING) -Wno-missing-include-dirs

# This makefile depends on devkitARM.

export PLATFORM ?= DS

ifeq ($(PLATFORM),DS)
	ifeq ($(strip $(DEVKITARM)),)
 		$(error "Please set DEVKITARM in your environment. \
export DEVKITARM=<path to>devkitARM")
	endif
	include $(DEVKITARM)/ds_rules
	export AR := $(PREFIX)gcc-ar
endif

ARCH_AS_ARM9 := -mthumb -mthumb-interwork -march=armv5te
ARCH_ARM9 := $(ARCH_AS_ARM9) -mtune=arm946e-s

# NOTE: arm9tdmi isn't the correct CPU arch, but anything newer and LD
# *insists* it has a FPU or VFP, and it won't take no for an answer!

ASFLAGS_ARM9 := $(ARCH_ARM9) -mfpu=softfpa -mfpu=softvfp

# XXX: NPLAMath does no support '-ffast-math'.
C_CXXFLAGS_ARM9 := $(C_CXXFLAGS_OPT_DBG) $(ARCH_ARM9) -Wno-psabi -DARM9

ARCH_AS_ARM7 := -mthumb -mthumb-interwork -mcpu=arm7tdmi
ARCH_ARM7 := $(ARCH_AS_ARM7) -mtune=arm7tdmi

ASFLAGS_ARM7 := $(ARCH_ARM7) -mfpu=softfpa -mfpu=softvfp

C_CXXFLAGS_ARM7 := $(C_CXXFLAGS_OPT_DBG) $(ARCH_ARM7) -Wno-psabi -ffast-math \
	-DARM7

