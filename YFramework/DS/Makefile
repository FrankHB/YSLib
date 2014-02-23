#
#	(C) 2009-2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YFramework -> ARM9
# Version = r2009
# Created = 2009-12-18 12:27:40 +0800
# Updated = 2014-02-23 08:45 +0800
# Encoding = ANSI


# This makefile is written based on PALib Makefile by Scognito and Tom and crash.

#----
.SUFFIXES:
#----
export PLATFORM		?= DS

ifeq ($(PLATFORM),DS)
	ifeq ($(strip $(DEVKITARM)),)
		$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
	endif
	include $(DEVKITARM)/ds_rules
endif

#----
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
# GRAPHICS is a list of directories containing image files to be converted with grit
#----

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
	$(REFDIR)/source/YSLib/Service $(REFDIR)/Source/YSLib/UI \
	$(REFDIR)/source/NPL $(REFDIR)/source/Helper \
	$(REFDIR)/DS/source/Helper
export INCLUDES		:=	include data ../../YBase/include $(REFDIR)/include ../../3rdparty/include
export DATA			:=	data $(REFDIR)/data
#export GRAPHICS	:=	gfx
#export MUSIC		:=	music

#----
# options for code generation
#----
ifeq ($(PLATFORM),DS)

ARCH_AS	:=	-mthumb -mthumb-interwork -march=armv5te
ARCH	:=	$(ARCH_AS) -mtune=arm946e-s

# NOTE: arm9tdmi isn't the correct CPU arch, but anything newer and LD
# *insists* it has a FPU or VFP, and it won't take no for an answer!

ASFLAGS	:=	$(ARCH) -mfpu=softfpa -mfpu=softvfp

CFLAGS	:=	-Wall -Wextra -Wunreachable-code \
 			-Wmissing-declarations -Winvalid-pch \
 			-fdata-sections -ffunction-sections \
 			-ffast-math \
 			-flto=jobserver \
 			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9

ifneq ($(CONF),debug)
CFLAGS	+=	-O3 -fomit-frame-pointer -DNDEBUG
else
ASFLAGS	+=	-g
CFLAGS	+=	-g -O0 -fno-omit-frame-pointer
endif

CXXFLAGS	:=	$(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=c++11 \
 			-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsign-promo
# 			-Wctor-dtor-privacy -Wnoexcept -Wold-style-cast -Wsign-promo

endif

#----
# any extra libraries we wish to link with the project
#----
LIBS	:=	#-lfreetype #-lfat -lnds9

#----
# list of directories containing libraries, this must be the top level containing
# include and lib
#----
LIBDIRS	:=	$(LIBNDS) #lib

#----
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#----
ifneq ($(CONF),$(notdir $(CURDIR)))
#----

export OUTPUT	:=	$(RELEASEDIR)/lib$(TARGET).a

export DEPSDIR	:=	$(BUILDDIR)/dummy

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(DEPSDIR)

CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
SFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.s))
PCXFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.pcx))
BINFILES	:=	$(foreach dir,$(DATA),$(wildcard $(dir)/*.bin))

ifeq ($(strip $(AUDIOFILES)),)
	ifeq ($(strip $(ARM7_SELECTED)), ARM7_MAXMOD_DSWIFI)
 		TEXT_TO_SAY_2 := No MaxMod-compatible files were found in the audio folder.
 		TEXT_TO_SAY_3 := If you are loading the soundbank from FAT or EFS, ignore this message.
 		TEXT_TO_SAY_4 := " "
	endif
	DONT_BUILD_MAXMOD_SOUNDBANK = YES
endif

ifneq ($(strip $(DONT_BUILD_MAXMOD_SOUNDBANK)), YES)
	ifeq ($(strip $(ARM7_SELECTED)), ARM7_MAXMOD_DSWIFI)
		BINFILES += soundbank_bin.bin
	endif
endif

PNGFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.png))
PALFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.pal))
RAWFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.raw))
MAPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.map))
JPEGFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.jpg))
MODFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.mod))
GIFFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.gif))
BMPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.bmp))
MP3FILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.mp3))

#----
# use CXX for linking C++ projects, CC for standard C
#----
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(MAPFILES)) $(addsuffix .o,$(RAWFILES)) \
	$(addsuffix .o,$(PALFILES)) $(addsuffix .o,$(BINFILES)) \
	$(addsuffix .o,$(MODFILES)) $(addsuffix .o,$(BMPFILES)) \
		$(addsuffix .o,$(JPEGFILES)) $(addsuffix .o,$(GIFFILES)) \
		$(addsuffix .o,$(PNGFILES)) \
	$(addsuffix .o,$(PCXFILES)) $(addsuffix .o,$(MP3FILES)) \
	$(addsuffix .o,$(CPPFILES)) $(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include/nds) \
	-I$(CURDIR)/$(CONF)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(CONF) clean rebuild all

#----
$(CONF):
	@echo Building configuration: $(notdir $@) ...
	@[ -d $(BUILDDIR)/dummy ] || mkdir -p $(BUILDDIR)/dummy
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)
	@$(MAKE) --no-print-directory -C $(BUILDDIR) -f $(CURDIR)/Makefile
	@echo Built configuration: $(notdir $@).

#----
clean:
	@echo Cleaning $(TARGET) of $(PLATFORM) ...
	@rm -fr $(BUILDDIR)/*
	@rm -f $(OUTPUT)
	@echo Cleaned.

rebuild: all

all: $(CONF) $(OUTPUT)

#----
else

DEPENDS	:= $(OFILES:.o=.d)

#----
# main targets
#----

$(OUTPUT)	:	$(OFILES)
	@rm -f "$(OUTPUT)"
	@echo Linking $(notdir $@) ...
	@$(AR) rcs "$(OUTPUT)" $(foreach file,$(OFILES),$(DEPSDIR)/$(file))

#----
# you need a rule like this for each extension you use as binary data
#----

#----
# Compile Targets for C/C++
#----

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

%.mp3.o	:	%.mp3
	@echo $<
	@$(bin2o)

%.pcx.o	:	%.pcx
	@echo $<
	@$(bin2o)

%.bin.o	:	%.bin
	@echo $<
	@$(bin2o)

%.png.o	:	%.png
	@echo $<
	@$(bin2o)

%.raw.o	:	%.raw
	@echo $<
	@$(bin2o)

%.pal.o	:	%.pal
	@echo $<
	@$(bin2o)

%.map.o	:	%.map
	@echo $<
	@$(bin2o)

%.mdl.o	:	%.mdl
	@echo $<
	@$(bin2o)

%.jpg.o	:	%.jpg
	@echo $<
	@$(bin2o)

%.mod.o	:	%.mod
	@echo $<
	@$(bin2o)

%.gif.o	:	%.gif
	@echo $<
	@$(bin2o)

%.bmp.o	:	%.bmp
	@echo $<
	@$(bin2o)

#----
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#----
%.s %.h   : %.png %.grit
	grit $< -fts -o$*

-include $(foreach file,$(DEPENDS),$(DEPSDIR)/$(file))

#----
endif
#----

