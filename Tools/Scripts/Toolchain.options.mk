#
#	(C) 2014-2015 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile specifying common toolchain options
# Version = r71
# Created = 2014-12-06 17:24:04 +0800
# Updated = 2015-03-29 09:40 +0800
# Encoding = ANSI


# This makefile depends on G++.

# See also %SHBuild-common-options.sh.
# TODO: Add %C_CXXFLAGS_GC.

C_CXXFLAGS_COMMON ?= -pipe -fdata-sections -ffunction-sections -pedantic-errors

C_CXXFLAGS_WARNING ?= -Wall -Wcast-align -Wdeprecated \
	-Wdeprecated-declarations -Wextra -Wfloat-equal -Wformat=2 -Winvalid-pch \
	-Wmissing-declarations -Wmissing-include-dirs -Wmultichar \
	-Wno-format-nonliteral -Wpacked -Wredundant-decls -Wshadow \
	-Wsign-conversion

CFLAGS_STD ?= -std=c11

# TODO: Detect implementation.
C_CXXFLAGS_IMPL_WARNING ?= -Wdouble-promotion -Wlogical-op -Wtrampolines
CXXFLAGS_IMPL_WARNING ?= -Wconditionally-supported -Wstrict-null-sentinel \
	-Wzero-as-null-pointer-constant

CFLAGS_WARNING ?= $(C_CXXFLAGS_WARNING) $(C_CXXFLAGS_IMPL_WARNING)

CFLAGS_COMMON ?= $(C_CXXFLAGS_COMMON) $(CFLAGS_STD) $(CFLAGS_WARNING)

CXXFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto=jobserver
LDFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto

CXXFLAGS_STD ?= -std=c++11
CXXFLAGS_WARNING ?= $(CFLAGS_WARNING) -Wctor-dtor-privacy -Wnon-virtual-dtor \
	-Woverloaded-virtual -Wsign-promo $(C_CXXFLAGS_IMPL_WARNING) \
	$(CXXFLAGS_IMPL_WARNING)

CXXFLAGS_COMMON ?= $(C_CXXFLAGS_COMMON) $(CXXFLAGS_STD) \
	$(CXXFLAGS_WARNING) $(CXXFLAGS_IMPL_COMMON)

ifneq ($(CONF),debug)
	ASFLAGS_OPT_DBG ?=
	C_CXXFLAGS_OPT_LV ?= -O3
	C_CXXFLAGS_OPT_DBG ?= $(C_CXXFLAGS_OPT_LV) -fomit-frame-pointer \
		$(CXXFLAGS_IMPL_OPT) -DNDEBUG
	LDFLAGS_OPT_DBG ?= $(LDFLAGS_IMPL_OPT)
else
	ASFLAGS_OPT_DBG ?= -g
	C_CXXFLAGS_OPT_LV ?= -O0
	C_CXXFLAGS_OPT_DBG ?= $(C_CXXFLAGS_OPT_LV) -g -fno-omit-frame-pointer \
		-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
	LDFLAGS_OPT_DBG ?= -g
endif

