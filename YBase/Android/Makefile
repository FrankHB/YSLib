#
#	(C) 2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# Android Makefile for YBase
# Version = r213
# Created = 2014-04-07 12:41:43 +0800
# Updated = 2014-11-02 15:22 +0800
# Encoding = ANSI


# This makefile is written based on DS Makefile.

#----
.SUFFIXES:
#----
export PLATFORM		?= Android

export PREFIX ?= arm-linux-androideabi-

export CC	:=	$(PREFIX)gcc
export CXX	:=	$(PREFIX)g++
export AS	:=	$(PREFIX)as
export AR	:=	$(PREFIX)ar
export OBJCOPY	:=	$(PREFIX)objcopy
export STRIP	:=	$(PREFIX)strip
export NM	:=	$(PREFIX)nm

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
#----

export PROJNAME		?=	YBase
export CONF			?=	release
export RELEASE		=	$(CONF)
export TOPDIR		?=	$(CURDIR)
export BUILDDIR		?=	$(TOPDIR)/../../build/$(PLATFORM)/$(PROJNAME)/$(RELEASE)
export RELEASEDIR	?=	$(BUILDDIR)
export TARGET		:=	$(PROJNAME)
export REFDIR		:=	..
export SOURCES		:=	source data $(REFDIR)/source \
	$(REFDIR)/source/ystdex $(REFDIR)/source/libdefect $(REFDIR)/source/ytest
export INCLUDES		:=	include data $(REFDIR)/include
export DATA			:=	data $(REFDIR)/data

#----
# options for code generation
#----
ifeq ($(PLATFORM),Android)

ARCH_AS	:=	-mthumb -march=armv5te
ARCH	:=	$(ARCH_AS)

ASFLAGS	:=	$(ARCH)

CFLAGS	:=	-Wall -Wextra -Wcast-align -Winvalid-pch -Wmissing-declarations \
			-Wredundant-decls -Wunreachable-code \
 			-fdata-sections -ffunction-sections \
 			-ffast-math -flto=jobserver \
 			$(ARCH)

CFLAGS	+=	$(INCLUDE) -pedantic-errors

ifneq ($(CONF),debug)
CFLAGS	+=	-O3 -fomit-frame-pointer -DNDEBUG
else
ASFLAGS	+=	-g
CFLAGS	+=	-g -O0 -fno-omit-frame-pointer
endif

CXXFLAGS	:=	$(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=c++11 \
 			-Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast \
 			-Wsign-promo

endif

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
BINFILES	:=	$(foreach dir,$(DATA),$(wildcard $(dir)/*.bin))

MAPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.map))

#----
# use CXX for linking C++ projects, CC for standard C
#----
ifeq ($(strip $(CPPFILES)),)
 	export LD	:=	$(CC)
else
 	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(MAPFILES)) $(addsuffix .o,$(BINFILES)) \
	$(addsuffix .o,$(CPPFILES)) $(addsuffix .o,$(CFILES)) $(addsuffix .o,$(SFILES))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
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

%.bin.o	:	%.bin
	@echo $<
	@$(bin2o)

%.map.o	:	%.map
	@echo $<
	@$(bin2o)

-include $(foreach file,$(DEPENDS),$(DEPSDIR)/$(file))

#----
endif
#----

