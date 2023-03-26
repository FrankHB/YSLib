#
#	(C) 2014-2015, 2017, 2020-2021, 2023 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile specifying common toolchain options
# Version = r85
# Created = 2014-12-06 17:24:04 +0800
# Updated = 2023-03-26 12:22 +0800
# Encoding = ANSI


# This makefile depends on G++.

# See also %SHBuild-common-options.sh.
# TODO: Add %C_CXXFLAGS_GC.

C_CXXFLAGS_COMMON ?= -pipe -fdata-sections -ffunction-sections -pedantic-errors

C_CXXFLAGS_WARNING ?= -Wall -Wcast-align -Wdeprecated \
	-Wdeprecated-declarations -Wextra -Wfloat-equal -Wformat=2 -Winvalid-pch \
	-Wmissing-declarations -Wmissing-include-dirs -Wmultichar \
	-Wno-format-nonliteral -Wredundant-decls -Wshadow -Wsign-conversion

CFLAGS_STD ?= -std=c11

# TODO: Detect implementation.
C_CXXFLAGS_IMPL_WARNING ?= -Wdouble-promotion -Wlogical-op -Wtrampolines
CXXFLAGS_IMPL_WARNING ?= -Wconditionally-supported -Wno-ignored-attributes \
	-Wno-noexcept-type -Wstrict-null-sentinel -Wzero-as-null-pointer-constant

CFLAGS_WARNING ?= $(C_CXXFLAGS_WARNING) $(C_CXXFLAGS_IMPL_WARNING)

CXXFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto=jobserver
LDFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto

CXXFLAGS_STD ?= -std=c++11
CXXFLAGS_WARNING ?= $(CFLAGS_WARNING) -Wctor-dtor-privacy -Wnon-virtual-dtor \
	-Woverloaded-virtual -Wsign-promo $(C_CXXFLAGS_IMPL_WARNING) \
	$(CXXFLAGS_IMPL_WARNING)

CXXFLAGS_IMPL_COMMON ?= \
	-U__GXX_TYPEINFO_EQUALITY_INLINE -D__GXX_TYPEINFO_EQUALITY_INLINE=1 \
	-U__GXX_MERGED_TYPEINFO_NAMES -D__GXX_MERGED_TYPEINFO_NAMES=1

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
		-D_GLIBCXX_DEBUG_PEDANTIC
	LDFLAGS_OPT_DBG ?= -g
endif

