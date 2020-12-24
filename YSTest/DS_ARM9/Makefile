#
#	(C) 2009-2016, 2020 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YShell Test -> ARM9
# Version = r1533
# Created = 2009-12-18 12:27:40 +0800
# Updated = 2020-12-14 02:42 +0800
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

export PROJNAME		?=	YSTest_ARM9
export CONF			?=	release
export RELEASE		=	$(CONF)
export TOPDIR		?=	$(CURDIR)
export BUILDDIR		?=	$(TOPDIR)/../../build/$(PLATFORM)/$(PROJNAME)/$(RELEASE)
export RELEASEDIR	?=	$(BUILDDIR)
export TARGET		:=	$(shell basename $(CURDIR))
export SOURCES		:=	source data ../source ../data
export INCLUDES		:=	include data ../include ../data ../DS/include \
	../DS/data ../../YFramework/include ../../YFramework/DS/include \
	../../3rdparty/include ../../3rdparty/freetype/include ../../YBase/include
export DATA			:=	data
#export GRAPHICS	:=	gfx
#export MUSIC		:=	music

# options for code generation
ARCH := $(ARCH_ARM9)
ARCH_AS := $(ARCH_AS_ARM9)
ASFLAGS := $(ASFLAGS_ARM9)
# NOTE: Options '-Wno-redundant-decls' and '-Wno-ignored-qualifiers' are the
#	workaround for libnds.
CFLAGS := $(CFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS_ARM9) $(C_CXXFLAGS_COMMON) \
	$(CFLAGS_WARNING) -Wno-redundant-decls -Wno-ignored-qualifiers
CXXFLAGS := $(CXXFLAGS_STD) $(INCLUDE) $(C_CXXFLAGS_ARM9) $(C_CXXFLAGS_COMMON) \
	$(CXXFLAGS_WARNING) $(CXXFLAGS_IMPL_COMMON) -fno-threadsafe-statics \
	-fuse-cxa-atexit -Wno-redundant-decls -Wno-ignored-qualifiers
#	-Wnoexcept -Wold-style-cast

LDFLAGS := $(LDFLAGS_OPT_DBG) -specs=ds_arm9.specs $(ARCH) -Wl,--gc-sections

#CFLAGS	:=	-g -Wformat=2 -Winline -Wall \
#		-mcpu=arm946e-s -mtune=arm946e-s -fomit-frame-pointer \
#		-ffast-math \
#		$(ARCH)
#CFLAGS	+=	$(INCLUDE) -I$(PAPATH)/include/nds
#CXXFLAGS	:=	$(CFLAGS) -frepo -fno-threadsafe-statics
#CXXFLAGS	:=	$(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=gnu++0x
#	-Weffc++ -Wold-style-cast -Woverloaded-virtual -pedantic
#	-Wextra -Winit-self -Wmissing-include-dirs -Wsign-promo

#LDFLAGS=	-specs=ds_arm9.specs -g $(ARCH) -L$(PAPATH)/lib \
#	-Wl,-Map,$(notdir $*.map)

LIBS := -lnds9

# User defined libraries.
LIBPATHEX	:= -L$(TOPDIR)/../../YFramework/DS/lib
LIBEX		:= -lFreeImage -lfreetype #-lagg -lloki

YBASE_A := $(BUILDDIR)/../../YBase/$(RELEASE)/libYBase.a
YFRAMEWORK_A := $(BUILDDIR)/../../YFramework/$(RELEASE)/libYFramework.a

LIBDIRS	:=	$(LIBNDS)

ifneq ($(CONF),$(notdir $(CURDIR)))

export ARM9ELF	:=	$(RELEASEDIR)/$(PROJNAME).arm9.elf
export DEPSDIR	:=	$(BUILDDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(CURDIR)/. $(DEPSDIR)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES := $(BINFILES:.bin=.o) $(CPPFILES:.cpp=.o) \
	$(CFILES:.c=.o) $(SFILES:.s=.o)

#export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
#			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
#			-I$(CURDIR)/$(CONF)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include/nds) \
					-I$(CURDIR)/$(CONF)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(CONF) rebuild clean

$(CONF):
	@echo Building configuration: $(notdir $@) ...
	@[ -d $(BUILDDIR) ] || mkdir -p $(BUILDDIR)
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)
	@$(MAKE) --no-print-directory -C $(BUILDDIR) -f $(CURDIR)/Makefile
	@echo Built configuration: $(notdir $@) .

clean:
	@echo Cleaning $(TARGET) ...
	@rm -f $(BUILDDIR)/*.d
	@rm -f $(BUILDDIR)/*.o
	@rm -f $(ARM9ELF)

rebuild: all

all: $(CONF) $(OUTPUT)

else

DEPENDS	:= $(OFILES:.o=.d)

$(ARM9ELF)	:	$(OFILES) $(YFRAMEWORK_A) $(YBASE_A)
	@echo Linking $(notdir $@) ...
	$(LD) $(LDFLAGS) $(OFILES) $(YFRAMEWORK_A) $(YBASE_A) $(LIBPATHS) $(LIBS) $(LIBPATHEX) $(LIBEX) -o $@
	@echo Built $(notdir $@) .

%.o : %.cpp
	@echo $(notdir $<)
	@$(CXX) -MM $(CXXFLAGS) -o $*.d $<
	@$(CXX) $(CXXFLAGS) -c $< -o$@

%.o : %.c
	@echo $(notdir $<)
	@$(CC) -MM $(CFLAGS) -o $*.d $<
	@$(CC)  $(CFLAGS) -c $< -o$@

%.o : %.s
	@echo $(notdir $<)
	@$(CC) -MM $(CFLAGS) -o $*.d $<
	@$(CC)  $(ASFLAGS) -c $< -o$@

define bin2o
	cp $(<) $(notdir $(*))
	bin2s $(notdir $(*)) | $(AS) $(ARCH_AS) -o $(DEPSDIR)/$(@)
	rm $(notdir $(*))

	echo "extern const unsigned char" $(*)"[];" > $(*).h
	echo "extern const unsigned" $(*)_size";" >> $(*).h
endef

%.o : %.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d

endif

