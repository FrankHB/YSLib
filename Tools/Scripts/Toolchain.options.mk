#
#	(C) 2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile specifying common toolchain options
# Version = r52
# Created = 2014-12-06 17:24:04 +0800
# Updated = 2014-12-06 21:55 +0800
# Encoding = ANSI


# This makefile depends on G++.

# See also %SHBuild-common-options.sh.

C_CXXFLAGS_COMMON ?= -pipe -fdata-sections -ffunction-sections -pedantic-errors

CFLAGS_STD ?= -std=c11
CFLAGS_WARNING ?= -Wall -Wcast-align -Wextra -Winit-self -Winvalid-pch \
	-Wmain -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls \
	-Wsign-promo -Wunreachable-code

CFLAGS_COMMON ?= $(C_CXXFLAGS_COMMON) $(CFLAGS_STD) $(CFLAGS_WARNING)

CXXFLAGS_IMPL_WARNING := -Wzero-as-null-pointer-constant
CXXFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto=jobserver
LDFLAGS_IMPL_OPT ?= -s -fexpensive-optimizations -flto

CXXFLAGS_STD ?= -std=c++11
CXXFLAGS_WARNING ?= $(CFLAGS_WARNING) -Wctor-dtor-privacy -Wnon-virtual-dtor \
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
	C_CXXFLAGS_OPT_DBG ?= -g $(C_CXXFLAGS_OPT_LV) -fno-omit-frame-pointer
	LDFLAGS_OPT_DBG ?= -g
endif

