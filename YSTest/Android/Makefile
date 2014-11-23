#
#	(C) 2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Android Makefile for YSTest
# Version = r244
# Created = 2014-04-17 23:51:27 +0800
# Updated = 2014-11-21 10:18 +0800
# Encoding = ANSI

.SUFFIXES:
export PLATFORM		?= Android
ifeq ($(PLATFORM),Android)
	ifeq ($(strip $(ANDROID_SDK)),)
		$(error "Please set ANDROID_SDK in your environment. export ANDROID_SDK=<path to>ANDROID_SDK")
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

export PREFIX ?= arm-linux-androideabi-

export CC := $(PREFIX)gcc
export CXX := $(PREFIX)g++
export AS := $(PREFIX)as
export AR := $(PREFIX)gcc-ar
export OBJCOPY := $(PREFIX)objcopy
export STRIP := $(PREFIX)strip
export NM := $(PREFIX)nm

# PROJNAME is the name of the project
# CONF is the configuration name
# RELEASE is the directory name for object files and intermediate files will be placed
# TOPDIR is the top directory path of the project
# BUILDDIR is the path for intermediate files
# RELEASEDIR is the path for target files
# TARGET is the name of the file to output
# OUTPUT is the path of output without extension
# SOARCH is the last subdirectory name of output library path
# OUTPUTSODIR is the directory path of output library
# OUTPUTSO is the path of output library with prefix
# OUTPUT is the path of apk file
# OUTPUT_SIGNED is the path of signed apk file
# OUTPUT_FINAL is the path of final apk file
export PROJNAME ?= YSTest
export CONF ?= release
export RELEASE = $(CONF)
export TOPDIR ?= $(CURDIR)
export BUILDDIR ?= $(TOPDIR)/../../build/$(PLATFORM)/$(PROJNAME)/$(RELEASE)
export RELEASEDIR ?= $(BUILDDIR)
export TARGET := $(PROJNAME)
export OUTPUT := $(BUILDDIR)/$(TARGET)
export SOARCH := armeabi
export OUTPUTSODIR := $(BUILDDIR)/$(SOARCH)
export OUTPUTSO := $(OUTPUTSODIR)/lib$(TARGET).so
export OUTPUT_APK := $(OUTPUT).apk
export OUTPUT_SIGNED := $(BUILDDIR)/$(TARGET).signed.apk
export OUTPUT_FINAL := $(BUILDDIR)/$(TARGET).final.apk

# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# RESDIR is the root directory of resources
# MANIFEST is the manifext XML file
export SOURCES := source ../source
export INCLUDES := include ../include ../Android/include \
	../../YFramework/include ../../YFramework/Android/include ../../YFramework/DS/include \
	../../3rdparty/include ../../YBase/include
export RESDIR := $(TOPDIR)/data
export MANIFEST := $(TOPDIR)/AndroidManifest.xml

RESOURCES := $(BUILDDIR)/../../$(PROJNAME)/$(CONF)/resources.ap_

# options for code generation
ARCH_AS := -mthumb -march=armv5te
ARCH := $(ARCH_AS)

ASFLAGS := $(ARCH)

CFLAGS := -Wall -Wextra -Winvalid-pch -Wmissing-declarations \
	-Wredundant-decls -Wunreachable-code \
 	-fdata-sections -ffunction-sections \
 	-ffast-math $(ARCH)

CFLAGS += $(INCLUDE)
LDFLAGS := $(ARCH) -Wl,--gc-sections -Wl,-Map,$(notdir $*.map) \
	-Wl,--no-undefined -Wl,--dynamic-list-data \
	-Wl,--dynamic-list-cpp-new -Wl,--dynamic-list-cpp-typeinfo

ifneq ($(CONF),debug)
CFLAGS += -O3 -fomit-frame-pointer -flto=jobserver -DNDEBUG
LDFLAGS += -s
else
ASFLAGS += -g
CFLAGS += -g -O0 -fno-omit-frame-pointer
LDFLAGS += -g
endif

CXXFLAGS := $(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=c++11 \
 	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsign-promo
# 	-Wctor-dtor-privacy -Wold-style-cast -Wsign-promo

# any extra libraries we wish to link with the project
LIBS :=-llog -landroid -lEGL -lGLESv1_CM
#LIBS := -landroid -llog -lEGL -lGLESv2
# User defined libraries.
LIBPATHEX := -L$(TOPDIR)/../../YFramework/Android/lib
LIBEX := -lfreetype #-lFreeImage

YBASE_A := $(BUILDDIR)/../../YBase/$(RELEASE)/libYBase.a
YFRAMEWORK_A := $(BUILDDIR)/../../YFramework/$(RELEASE)/libYFramework.a

# list of directories containing libraries, this must be the top level containing
# include and lib
LIBDIRS	:=

ifneq ($(RELEASE),$(notdir $(CURDIR)))

export DEPSDIR := $(BUILDDIR)/dummy
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
	$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
	$(CURDIR)/. $(DEPSDIR)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

# use CXX for linking C++ projects, CC for standard C
ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export OFILES := $(addsuffix .o,$(MAPFILES)) $(addsuffix .o,$(BINFILES)) \
	$(addsuffix .o,$(CPPFILES)) $(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
	-I$(CURDIR)/$(CONF)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(CONF) clean rebuild all

$(CONF):
	@echo Building configuration: $(notdir $@) ...
	@[ -d $(RESDIR) ] || mkdir -p $(RESDIR)
	@[ -d $(BUILDDIR)/dummy ] || mkdir -p $(BUILDDIR)/dummy
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)
	@[ -d $(OUTPUTSODIR) ] || mkdir -p $(OUTPUTSODIR)
	@$(MAKE) --no-print-directory -C $(BUILDDIR) -f $(CURDIR)/Makefile
	@echo Built configuration: $(notdir $@).

clean:
	@echo Cleaning releases ...
	@rm -f $(foreach file,$(OFILES:.o=.d),$(DEPSDIR)/$(file))
	@rm -f $(foreach file,$(OFILES),$(DEPSDIR)/$(file))
	@rm -f $(BUILDDIR)/.map
	@rm -fr $(OUTPUTSODIR)
	@rm -f $(RESOURCES)
	@rm -f $(OUTPUT_FINAL) $(OUTPUT_SIGNED) $(OUTPUT_APK)
	@echo Cleaned.

rebuild: all

# main targets
all: $(CONF) $(OUTPUT_FINAL)

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT_FINAL) : $(OUTPUT_SIGNED)
	@echo Aligning $(notdir $@) ...
	$(ANDROID_BUILD_TOOLS_DIR)/zipalign -f -v 4 $(OUTPUT_SIGNED) $(OUTPUT_FINAL)
	@echo Built $(notdir $@) .

$(OUTPUT_SIGNED) : $(OUTPUT_APK)
	@echo Signing $(notdir $@) ...
	$(JARSIGNER) -verbose $(JARSIGN_ALGO) -keystore $(APK_KEYSTORE) $(APK_PASSWORD_OPT) -signedjar $(OUTPUT_SIGNED) $(OUTPUT_APK) $(APK_KEYSTORE_ALIAS)
	@echo Signed $(notdir $@) .

$(OUTPUT_APK) : $(RESOURCES) $(OUTPUTSO)
	@echo Building $(notdir $@) ...
	$(JAVA) -classpath $(ANDROID_SDK)/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain $(OUTPUT_APK) -v -u -z $(RESOURCES) -nf $(BUILDDIR)
	@echo Built $(notdir $@) .

$(RESOURCES) : $(MANIFEST)
	$(ANDROID_BUILD_TOOLS_DIR)/aapt package -f -m -J $(TOPDIR)/gen -S $(RESDIR) -M $(MANIFEST) -I $(platforms)/android.jar
	$(ANDROID_BUILD_TOOLS_DIR)/aapt package -f -M $(MANIFEST) -S $(RESDIR) -I $(platforms)/android.jar -F $(RESOURCES)

$(OUTPUTSO) : $(OFILES) $(YFRAMEWORK_A) $(YBASE_A)
	@rm -f "$(OUTPUTSO)"
	@echo Linking $(notdir $@) ...
	$(LD) -shared $(LDFLAGS) $(foreach file,$(OFILES),$(DEPSDIR)/$(file)) $(YFRAMEWORK_A) $(YBASE_A) $(LIBPATHS) $(LIBS) $(LIBPATHEX) $(LIBEX) -o $@

%.cpp.o : %.cpp
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.cpp.d $(CXXFLAGS) -c $< -o$(DEPSDIR)/$@

%.c.o : %.c
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.c.d $(CFLAGS) -c $< -o$(DEPSDIR)/$@

%.s.o : %.s
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.s.d -x assembler-with-cpp $(ASFLAGS) -c $< -o$(DEPSDIR)/$@

-include $(foreach file,$(DEPENDS),$(DEPSDIR)/$(file))

endif

