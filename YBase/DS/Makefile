#
#	(C) 2009-2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YBase
# Version = r1876
# Created = 2009-12-18 12:27:40 +0800
# Updated = 2014-01-18 15:23 +0800
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


export TOPDIR		?=	$(CURDIR)

#----
# TARGET is the name of the file to output
# BUILD is the configuration name
# RELEASE is the directory where object files and intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o
# GRAPHICS is a list of directories containing image files to be converted with grit
#----

export TARGET		:= YBase
export TARGETP		:= $(TARGET)_$(PLATFORM)
export REFDIR		:= ..
export BUILD		?= release
export RELEASE		= $(BUILD)
export SOURCES		:= source data $(REFDIR)/source $(REFDIR)/source/ystdex
export INCLUDES		:= include data $(REFDIR)/include
export DATA			:= data $(REFDIR)/data

#----
# options for code generation
#----
ifeq ($(PLATFORM),DS)

ARCH_AS	:=	-mthumb -mthumb-interwork -march=armv5te
ARCH	:=	$(ARCH_AS) -mtune=arm946e-s

# NOTE: arm9tdmi isn't the correct CPU arch, but anything newer and LD
# *insists* it has a FPU or VFP, and it won't take no for an answer!

ASFLAGS	:=	$(ARCH) -mfpu=softfpa -mfpu=softvfp

CFLAGS	:=	-Wall -Wextra \
 			-Wcast-align -Wunreachable-code \
 			-Wmissing-declarations -Wredundant-decls -Winvalid-pch \
 			-fdata-sections -ffunction-sections \
 			-ffast-math \
 			-flto=jobserver \
 			$(ARCH)

CFLAGS	+=	$(INCLUDE) -pedantic-errors -DARM9

ifneq ($(BUILD),debug)
CFLAGS	+=	-O3 -fomit-frame-pointer -DNDEBUG
else
ASFLAGS	+=	-g
CFLAGS	+=	-g -O0 -fno-omit-frame-pointer
endif

CXXFLAGS	:=	$(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=c++11 \
 			-Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wold-style-cast \
 			-Wsign-promo

endif

#----
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#----
ifneq ($(BUILD),$(notdir $(CURDIR)))
#----

export OUTPUT	:=	$(CURDIR)/$(RELEASE)/lib$(TARGET).a

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

MAPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.map)))

#----
# use CXX for linking C++ projects, CC for standard C
#----
ifeq ($(strip $(CPPFILES)),)
 	export LD	:=	$(CC)
else
 	export LD	:=	$(CXX)
endif

#export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
#					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES	:=	$(MAPFILES:.map=.o) $(BINFILES:.bin=.o)\
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#----
$(BUILD):
	@echo Building configuration: $(notdir $@) ...
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@echo Built configuration: $(notdir $@).

#----
clean:
	@echo Cleaning $(TARGETP) ...
	@rm -f $(BUILD)/*.h
	@rm -f $(BUILD)/*.d
	@rm -f $(BUILD)/*.o
	@rm -f $(OUTPUT)
	@echo Cleaned.

rebuild: all

all: $(BUILD) $(OUTPUT)

#----
else

DEPENDS	:= $(OFILES:.o=.d)

#----
# main targets
#----

$(OUTPUT)	:	$(OFILES)
	@rm -f "$(OUTPUT)"
	@echo Linking $(notdir $@) ...
	@$(AR) rcs "$(OUTPUT)" $(OFILES)

#----
# you need a rule like this for each extension you use as binary data
#----

#----
# Compile Targets for C/C++
#----

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


# bin2o macro like that from libnds, slightly changed for the PALib naming conventions of resources which doesn't append the file extension to the resource name
define bin2o
	cp $(<) $(*)
	bin2s $(*) | $(AS) $(ARCH_AS) -o $(@)
	rm $(*)

	echo "extern const u8" $(*)"[];" > $(*).h
	echo "extern const u32" $(*)_size";" >> $(*).h
endef

%.o	:	%.bin
	@echo $(notdir $<)
	@$(bin2o)

%.o	:	%.map
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d

#----
endif
#----

