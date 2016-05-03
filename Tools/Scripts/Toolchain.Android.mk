#
#	(C) 2014, 2016 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Makefile for Android toolchain
# Version = r65
# Created = 2014-12-06 16:51:13 +0800
# Updated = 2016-05-01 21:49 +0800
# Encoding = ANSI


include $(dir $(lastword $(MAKEFILE_LIST)))/Toolchain.options.mk

# NOTE: Wordaround for client makefiles.
CFLAGS_WARNING := $(CFLAGS_WARNING) -Wno-missing-include-dirs

# This makefile depends on Android SDK.

export PLATFORM ?= Android

export PREFIX ?= arm-linux-androideabi-

export CC := $(PREFIX)gcc
export CXX := $(PREFIX)g++
export AS := $(PREFIX)as
export AR := $(PREFIX)gcc-ar
export OBJCOPY := $(PREFIX)objcopy
export STRIP := $(PREFIX)strip
export NM := $(PREFIX)nm

ifeq ($(PLATFORM),Android)
	ifeq ($(strip $(ANDROID_SDK)),)
 		$(error "Please set ANDROID_SDK in your environment. \
			export ANDROID_SDK=<path to>ANDROID_SDK")
	endif
	ANDROID_PLATFORM := 9
	ANDROID_SDKVER := 21.1.1
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

ARCH_AS := -mthumb -march=armv5te
ARCH := $(ARCH_AS)

ASFLAGS	:= $(ARCH)

C_CXXFLAGS := $(C_CXXFLAGS_OPT_DBG) $(ARCH) -ffast-math

