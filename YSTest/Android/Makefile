#
#	(C) 2014, 2016, 2019-2020 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Android Makefile for YSTest
# Version = r364
# Created = 2014-04-17 23:51:27 +0800
# Updated = 2020-12-14 02:04 +0800
# Encoding = ANSI

.SUFFIXES:

SHBuild_ToolDir := $(dir $(lastword $(MAKEFILE_LIST)))/../../Tools/Scripts
include $(SHBuild_ToolDir)/Toolchain.Android.mk

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
#export SOARCH := armeabi
export SOARCH := armeabi-v7a
export OUTPUTSODIR := $(BUILDDIR)/$(SOARCH)
export OUTPUTSO := $(OUTPUTSODIR)/lib$(TARGET).so
NDK_ROOT := $(dir $(shell which $(CXX)))..
TARGET_LIB_PATH := $(NDK_ROOT)/sysroot/usr/lib/arm-linux-androideabi
TARGET_LIBV_PATH := $(TARGET_LIB_PATH)/$(ANDROID_PLATFORM)
#LIBCXXSOSRC := $(TARGET_LIB_PATH)/libc++_shared.so
#LIBCXXSO := $(OUTPUTSODIR)/libc++_shared.so
export OUTPUT_APK := $(OUTPUT).apk
export OUTPUT_SIGNED := $(BUILDDIR)/$(TARGET).signed.apk
export OUTPUT_FINAL := $(BUILDDIR)/$(TARGET).final.apk

# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# RESDIR is the root directory of resources
# MANIFEST is the manifext XML file
export SOURCES := source ../source
export INCLUDES := include ../include ../Android/include \
	../../YFramework/include ../../YFramework/Android/include \
	../../YFramework/DS/include ../../3rdparty/include \
	../../3rdparty/freetype/include ../../YBase/include
export RESDIR := $(TOPDIR)/data
export MANIFEST := $(TOPDIR)/AndroidManifest.xml

RESOURCES := $(BUILDDIR)/../../$(PROJNAME)/$(CONF)/resources.ap_

CFLAGS := $(CFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS) $(C_CXXFLAGS_COMMON) \
	$(CFLAGS_WARNING)
CXXFLAGS := $(CXXFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS) $(C_CXXFLAGS_COMMON) \
	$(CXXFLAGS_WARNING) $(CXXFLAGS_IMPL_COMMON) -fuse-cxa-atexit
#	-Wnoexcept -Wold-style-cast

LDFLAGS := $(LDFLAGS_OPT_DBG) $(ARCH) -Wl,--gc-sections \
	-Wl,--no-undefined

LIBS := -llog -landroid -lEGL -lGLESv1_CM
LIBCRYSTAX_STATIC ?=
#LIBCRYSTAX_STATIC ?= -u __crystax_on_load -u __crystax_on_unload \
	-Wl,-z,muldefs -Wl,--eh-frame-hdr -L$(BUILDDIR) -lstdc++ \
	$(TARGET_LIB_PATH)/libcrystax.a -lc
#LIBS := -landroid -llog -lEGL -lGLESv2
# User defined libraries.
LIBPATHEX := -L$(TOPDIR)/../../YFramework/Android/lib
LIBEX := -lfreetype #-lFreeImage

YBASE_A := $(BUILDDIR)/../../YBase/$(RELEASE)/libYBase.a
YFRAMEWORK_A := $(BUILDDIR)/../../YFramework/$(RELEASE)/libYFramework.a

LIBDIRS	:=

ifneq ($(RELEASE),$(notdir $(CURDIR)))

export DEPSDIR := $(BUILDDIR)/dummy
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
	$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
	$(CURDIR)/. $(DEPSDIR)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export OFILES := $(addsuffix .o,$(BINFILES)) $(addsuffix .o,$(CPPFILES)) \
	$(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))
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
	@rm -fr $(OUTPUTSODIR)
	@rm -f $(RESOURCES)
	@rm -f $(OUTPUT_FINAL) $(OUTPUT_SIGNED) $(OUTPUT_APK)
	@echo Cleaned.

rebuild: all

all: $(CONF) $(OUTPUT_FINAL)

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT_FINAL): $(OUTPUT_SIGNED)
	@echo Aligning $(notdir $@) ...
	$(ANDROID_BUILD_TOOLS_DIR)/zipalign -f -v 4 $(OUTPUT_SIGNED) $(OUTPUT_FINAL)
	@echo Built $(notdir $@) .

$(OUTPUT_SIGNED): $(OUTPUT_APK)
	@echo Signing $(notdir $@) ...
	$(JARSIGNER) -verbose $(JARSIGN_ALGO) -keystore $(APK_KEYSTORE) $(APK_PASSWORD_OPT) -signedjar $(OUTPUT_SIGNED) $(OUTPUT_APK) $(APK_KEYSTORE_ALIAS)
	@echo Signed $(notdir $@) .

$(OUTPUT_APK): $(RESOURCES) $(OUTPUTSO)
	@echo Building $(notdir $@) ...
	$(JAVA) -classpath $(ANDROID_SDK)/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain $(OUTPUT_APK) -v -u -z $(RESOURCES) -nf $(BUILDDIR)
	@echo Built $(notdir $@) .

$(RESOURCES): $(MANIFEST)
	$(ANDROID_BUILD_TOOLS_DIR)/aapt package -f -m -J $(TOPDIR)/gen -S $(RESDIR) -M $(MANIFEST) -I $(platforms)/android.jar
	$(ANDROID_BUILD_TOOLS_DIR)/aapt package -f -M $(MANIFEST) -S $(RESDIR) -I $(platforms)/android.jar -F $(RESOURCES)

# XXX: Since ANativeActivity does not support more than 1 shared libraries,
#	and libc++ is not the system library, this is not used.

#$(LIBCXXSOSRC):

#$(LIBCXXSO): $(LIBCXXSOSRC)
#	@echo Copying libc++ shared library file "$(LIBCXXSOSRC)" as needed...
#	@cp -f $(LIBCXXSOSRC) $(LIBCXXSO)
#	@echo Done.

$(OUTPUTSO): $(OFILES) $(YFRAMEWORK_A) $(YBASE_A) $(LIBCXXSO)
	@rm -f "$(OUTPUTSO)"
#	@rm -f "$(BUILDDIR)/libcrystax.a"
#	@echo Creating stub library for static linking ...
#	$(AR) rc "$(BUILDDIR)/libcrystax.a"
	@echo Linking $(notdir $@) ...
	$(LD) -shared -o $@ -static-libstdc++ -Wl,--exclude-libs,libgcc.a \
		-Wl,--exclude-libs,libunwind.a $(LDFLAGS) \
		$(foreach file,$(OFILES),$(DEPSDIR)/$(file)) \
		$(YFRAMEWORK_A) $(YBASE_A) $(LIBPATHS) $(LIBS) $(LIBPATHEX) $(LIBEX) \
		$(LIBCRYSTAX_STATIC)

%.cpp.o: %.cpp
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.cpp.d $(CXXFLAGS) -c $< -o$(DEPSDIR)/$@

%.c.o: %.c
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.c.d $(CFLAGS) -c $< -o$(DEPSDIR)/$@

%.s.o: %.s
	@echo $<
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.s.d -x assembler-with-cpp $(ASFLAGS) -c $< -o$(DEPSDIR)/$@

-include $(foreach file,$(DEPENDS),$(DEPSDIR)/$(file))

endif

