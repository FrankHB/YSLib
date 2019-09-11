#
#	(C) 2014, 2016-2017, 2019 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile for Android toolchain
# Version = r104
# Created = 2014-12-06 16:51:13 +0800
# Updated = 2019-09-11 21:22 +0800
# Encoding = ANSI


# NOTE: For Clang++.
C_CXXFLAGS_IMPL_WARNING ?= -Wdouble-promotion -Wno-assume
CXXFLAGS_IMPL_WARNING ?= -Wno-defaulted-function-deleted \
	-Wno-ignored-attributes -Wno-mismatched-tags -Wno-noexcept-type \
	-Wzero-as-null-pointer-constant
CXXFLAGS_IMPL_OPT ?= -flto
LDFLAGS_IMPL_OPT ?= -s -flto

include $(dir $(lastword $(MAKEFILE_LIST)))/Toolchain.options.mk

# NOTE: This is required by API level 21.
C_CXXFLAGS_COMMON := $(C_CXXFLAGS_COMMON) -fPIC

# NOTE: Wordaround for client makefiles.
CFLAGS_WARNING := $(CFLAGS_WARNING) -Wno-missing-include-dirs

# This makefile depends on Android SDK.

export PLATFORM ?= Android

ifeq ($(PLATFORM),Android)
	ifeq ($(strip $(ANDROID_SDK)),)
 		$(error "Please set ANDROID_SDK in your environment. \
			export ANDROID_SDK=<path to>ANDROID_SDK")
	endif
#	ANDROID_PLATFORM := 9
	ANDROID_PLATFORM := 21
	ANDROID_SDKVER := 29.0.2
#	TARGET_ARCH := arm
#	ANDROID_ABIVER :=
	TARGET_ARCH := armv7a
	ANDROID_ABIVER := $(ANDROID_PLATFORM)
#	java_exe := $(shell $(ANDROID_SDK)/tools/lib/find_java)
#	java_d := $(shell cygpath -d "$(java_exe)")
#	export JAVA := $(shell cygpath "$(java_d)")
	export JAVA := java
	export JARSIGNER := jarsigner
	# See https://code.google.com/p/android/issues/detail?id=19567 .
	export JARSIGN_ALGO ?= -digestalg SHA1 -sigalg MD5withRSA
	APK_KEYSTORE ?= ~/.android/debug.keystore
	APK_KEYSTORE_ALIAS ?= androiddebugkey
	APK_PASSWORD_OPT ?= -storepass android -keypass android
	ANDROID_BUILD_TOOLS_DIR := $(ANDROID_SDK)/build-tools/$(ANDROID_SDKVER)
	platforms := $(ANDROID_SDK)/platforms/android-$(ANDROID_PLATFORM)
endif

#export PREFIX ?= arm-linux-androideabi-
export PREFIX ?= $(TARGET_ARCH)-linux-androideabi$(ANDROID_ABIVER)-

export CC := $(PREFIX)clang
export CXX := $(PREFIX)clang++
export AS := $(PREFIX)as
# export AR := $(PREFIX)gcc-ar
export AR := llvm-ar
export OBJCOPY := $(PREFIX)objcopy
export STRIP := $(PREFIX)strip
export NM := $(PREFIX)nm

export TARGET_LIB_PATH := \
	$(dir $(shell which $(CC)))../arm-linux-androideabi/lib

# ARCH_AS := -mthumb -march=armv5te
ARCH_AS :=
ARCH := $(ARCH_AS)

ASFLAGS	:= $(ARCH)

C_CXXFLAGS := $(C_CXXFLAGS_OPT_DBG) $(ARCH) -ffast-math

