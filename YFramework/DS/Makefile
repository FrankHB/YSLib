#
#	(C) 2009-2016, 2020 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YFramework -> ARM9
# Version = r2186
# Created = 2009-12-18 12:27:40 +0800
# Updated = 2020-12-14 02:30 +0800
# Encoding = ANSI


# This makefile is written based on PALib Makefile by Scognito and Tom and crash.

.SUFFIXES:

SHBuild_ToolDir := $(dir $(lastword $(MAKEFILE_LIST)))/../../Tools/Scripts
include $(SHBuild_ToolDir)/Toolchain.DS.mk

# PROJNAME is the name of the project
# CONF is the configuration name
# RELEASE is the directory name for object files and intermediate files will be placed
# TOPDIR is the top directory path of the project
# BUILDDIR is the path for intermediate files
# RELEASEDIR is the path for target files
# TARGET is the name of the file to output
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o

export PROJNAME		?=	YFramework
export CONF			?=	release
export RELEASE		=	$(CONF)
export TOPDIR		?=	$(CURDIR)
export BUILDDIR		?=	$(TOPDIR)/../../build/$(PLATFORM)/$(PROJNAME)/$(RELEASE)
export RELEASEDIR	?=	$(BUILDDIR)
export TARGET		:=	$(PROJNAME)
export REFDIR		:=	..
export SOURCES		:=	source data \
	$(REFDIR)/source/CHRLib \
	$(REFDIR)/source/YCLib $(REFDIR)/source/YSLib \
	$(REFDIR)/source/YSLib/Adaptor $(REFDIR)/source/YSLib/Core \
	$(REFDIR)/source/YSLib/Service $(REFDIR)/source/YSLib/UI \
	$(REFDIR)/source/NPL $(REFDIR)/source/Helper \
	$(REFDIR)/DS/source/YCLib $(REFDIR)/DS/source/Helper
export INCLUDES		:=	include data ../../YBase/include $(REFDIR)/include \
	../../3rdparty/include ../../3rdparty/freetype/include
export DATA			:=	data $(REFDIR)/data

ARCH := $(ARCH_ARM9)
ARCH_AS := $(ARCH_AS_ARM9)
ASFLAGS := $(ASFLAGS_ARM9)
# NOTE: Options '-Wno-redundant-decls' and '-Wno-ignored-qualifiers' are the
#	workaround for libnds.
CFLAGS := $(CFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS_ARM9) $(C_CXXFLAGS_COMMON) \
	$(CFLAGS_WARNING) -Wno-cast-align -Wno-redundant-decls \
	-Wno-ignored-qualifiers
CXXFLAGS := $(CXXFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS_ARM9) $(C_CXXFLAGS_COMMON) \
	$(CXXFLAGS_WARNING) $(CXXFLAGS_IMPL_COMMON) -fno-threadsafe-statics \
	-fuse-cxa-atexit -Wno-cast-align -Wno-redundant-decls \
	-Wno-ignored-qualifiers
#	-Wnoexcept -Wold-style-cast

LIBS := #

LIBDIRS := $(LIBNDS) #lib

ifneq ($(CONF),$(notdir $(CURDIR)))

export OUTPUT	:=	$(RELEASEDIR)/lib$(TARGET).a

export DEPSDIR	:=	$(BUILDDIR)/dummy

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(CURDIR)/. $(DEPSDIR)

CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
SFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.s))
BINFILES	:=	$(foreach dir,$(DATA),$(wildcard $(dir)/*.bin))

ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES := $(addsuffix .o,$(BINFILES)) $(addsuffix .o,$(CPPFILES)) \
	$(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include/nds) \
	-I$(CURDIR)/$(CONF)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(CONF) clean rebuild all

$(CONF):
	@echo Building configuration: $(notdir $@) ...
	@[ -d $(BUILDDIR)/dummy ] || mkdir -p $(BUILDDIR)/dummy
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)
	@$(MAKE) --no-print-directory -C $(BUILDDIR) -f $(CURDIR)/Makefile
	@echo Built configuration: $(notdir $@).

clean:
	@echo Cleaning $(TARGET) of $(PLATFORM) ...
	@rm -f $(foreach file,$(OFILES:.o=.d),$(DEPSDIR)/$(file))
	@rm -f $(foreach file,$(OFILES),$(DEPSDIR)/$(file))
	@rm -f $(OUTPUT)
	@echo Cleaned.

rebuild: all

all: $(CONF) $(OUTPUT)

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT) : $(OFILES)
	@rm -f "$(OUTPUT)"
	@echo Linking $(notdir $@) ...
	@$(AR) rcs "$(OUTPUT)" $(foreach file,$(OFILES),$(DEPSDIR)/$(file))

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

define bin2o
	@[ -d $(dir $(DEPSDIR)/$@) ] || mkdir -p $(dir $(DEPSDIR)/$@)
	cp $(<) $(notdir $(*))
	bin2s $(notdir $(*)) | $(AS) $(ARCH_AS) -o $(DEPSDIR)/$(@)
	rm $(notdir $(*))

	echo "extern const u8" $(notdir $(*))"[];" > $(DEPSDIR)/$(*).h
	echo "extern const u32" $(notdir $(*))_size";" >> $(DEPSDIR)/$(*).h
endef

%.bin.o	: %.bin
	@echo $<
	@$(bin2o)

-include $(foreach file,$(DEPENDS),$(DEPSDIR)/$(file))

endif

