#
#	(C) 2009-2014 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YSTest by FrankHB 2009 - 2013
# Version = r1140
# Created = 2009-11-12 21:26:30 +0800
# Updated = 2014-12-06 22:04 +0800
# Encoding = ANSI

# This makefile is written based on devkitPro example template.

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
# OUTPUT is the path of output without extension

export PROJNAME		?=	YSTest
export CONF			?=	release
export RELEASE		=	$(CONF)
export TOPDIR		?=	$(CURDIR)
export BUILDDIR		?=	$(TOPDIR)/../../build/$(PLATFORM)/$(PROJNAME)/$(RELEASE)
export RELEASEDIR	?=	$(BUILDDIR)
export TARGET		:=	$(PROJNAME)
#export TARGET		:=	$(shell basename $(CURDIR))
export OUTPUT		=	$(BUILDDIR)/$(TARGET)

# Be sure to change these default banner TEXTs. This is the name your project will
# display in the DS menu (including some flash cards). Create a custom LOGO.BMP
# icon for your project, too!

TITLE		:= $(PROJNAME)
SUBTITLE1 	:= FrankHB
SUBTITLE2 	:= 2009 - 2014
ICON		:= -b $(CURDIR)/data/logo.bmp

# If you're using EFS_lib, uncomment "USE_EFS = YES" here.
#USE_EFS = YES

PROGNAME = $(PROJNAME)
OFILES +=
ADD_LIBS +=

ARM7OBJ := $(BUILDDIR)/../../$(TARGET)_ARM7/$(CONF)/$(TARGET)_ARM7.arm7
ARM9OBJ := $(BUILDDIR)/../../$(TARGET)_ARM9/$(CONF)/$(TARGET)_ARM9.arm9

.PHONY: $(CONF) clean rebuild

# main targets
all : $(CONF) $(OUTPUT).nds

%.ds.gba: %.nds
	@dsbuild $<
	@echo Built: $(notdir $(OUTPUT)).nds .
	@echo Built: $(notdir $@) .
#	@cp $(RELEASEDIR)/$(notdir $@) $(BUILDDIR)/$(notdir $(OUTPUT)).sc.nds
#	@echo Built: $(notdir $(OUTPUT)).sc.nds .
ifeq ($(strip $(USE_EFS)), YES)
	@$(CURDIR)/../efs $(OUTPUT).nds
#	@$(CURDIR)/../efs $(OUTPUT).ds.gba
#	@$(CURDIR)/../efs $(OUTPUT).sc.nds
endif

$(OUTPUT).nds	:	$(ARM9OBJ)
	@echo Building $(notdir $@) ...
ifeq ($(strip $(USE_EFS)), YES)
	ndstool -c $(OUTPUT).nds -7 $(ARM7OBJ) -9 $(ARM9OBJ) $(ICON) "$(TITLE);$(SUBTITLE1);$(SUBTITLE2)" -d $(TOPDIR)/efsroot
	$(TOPDIR)/efs.exe $(OUTPUT).nds
else
	ndstool -c $(OUTPUT).nds -7 $(ARM7OBJ) -9 $(ARM9OBJ) $(ICON) "$(TITLE);$(SUBTITLE1);$(SUBTITLE2)"
#	@cp $(OUTPUT).nds $(OUTPUT)_.nds
endif
	@echo Built $(notdir $@) .

$(CONF):
# Before compiling, show any warning messages that might have detected
ifneq ($(strip $(TEXT_TO_SAY_1)),)
ifeq ($(strip $(MSG_IS_ERROR)),)
	@echo " ===================================="
	@echo "/ YSLib Project Compilation Warning /"
	@echo "==================================== "
else
	@echo " =================================="
	@echo "/ YSLib Project Compilation Error /"
	@echo "================================== "
endif
	@echo.
	@echo $(TEXT_TO_SAY_1)
endif
ifneq ($(strip $(TEXT_TO_SAY_2)),)
	@echo $(TEXT_TO_SAY_2)
endif
ifneq ($(strip $(TEXT_TO_SAY_3)),)
	@echo $(TEXT_TO_SAY_3)
endif
ifneq ($(strip $(TEXT_TO_SAY_4)),)
	@echo $(TEXT_TO_SAY_4)
endif
ifneq ($(strip $(NL_AND_PAUSE)),)
	@echo.
ifeq ($(strip $(MSG_IS_ERROR)),)
	@read -n 1 -p "Press any key to continue the compilation..."
	@echo.
else
	@read -n 1 -p "Press any key to exit..."
endif
	@echo.
endif
ifeq ($(strip $(MSG_IS_ERROR)),)
	@[ -d $(BUILDDIR) ] || mkdir -p $(BUILDDIR)
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)
#	@make --no-print-directory -C $(BUILDDIR) -f $(CURDIR)/Makefile
endif

clean:
	@echo Cleaning releases ...
	@rm -f $(BUILDDIR)/*.d
	@rm -f $(BUILDDIR)/*.o
	@rm -f $(RELEASEDIR)/*.*ds*
	@echo Cleaned.

rebuild: all

