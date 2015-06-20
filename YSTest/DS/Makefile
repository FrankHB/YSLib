#
#	(C) 2009-2015 FrankHB.
#
#	This file is part of the YSLib project, and may only be used,
#	modified, and distributed under the terms of the YSLib project
#	license, LICENSE.TXT.  By continuing to use, modify, or distribute
#	this file you indicate that you have read the license and
#	understand and accept it fully.
#
# DS Makefile for YSTest by FrankHB
# Version = r1212
# Created = 2009-11-12 21:26:30 +0800
# Updated = 2015-06-19 18:59 +0800
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
export OUTPUT		=	$(RELEASEDIR)/$(TARGET)

# Be sure to change these default banner TEXTs. This is the name your project will
# display in the DS menu (including some flash cards). Create a custom LOGO.BMP
# icon for your project, too!

TITLE		:= $(PROJNAME)
SUBTITLE1 	:= FrankHB
SUBTITLE2 	:= 2009-2015
ICON		:= -b $(CURDIR)/data/logo.bmp

ARM7BIN := $(BUILDDIR)/../../$(TARGET)_ARM7/$(CONF)/$(TARGET)_ARM7.arm7
ARM9BIN := $(BUILDDIR)/../../$(TARGET)_ARM9/$(CONF)/$(TARGET)_ARM9.arm9

.PHONY: $(CONF) clean rebuild

# main targets
all : $(CONF)

%.ds.gba : %.nds
	@dsbuild $<
	@echo Built: $(notdir $(OUTPUT)).nds .
	@echo Built: $(notdir $@) .
#	@cp $(RELEASEDIR)/$(notdir $@) $(BUILDDIR)/$(notdir $(OUTPUT)).sc.nds
#	@echo Built: $(notdir $(OUTPUT)).sc.nds .

$(ARM7BIN) $(ARM9BIN) :
	@[ -d $(BUILDDIR) ] || mkdir -p $(BUILDDIR)
	@[ -d $(RELEASEDIR) ] || mkdir -p $(RELEASEDIR)

$(OUTPUT).nds : $(ARM7BIN) $(ARM9BIN)
	@echo Building configuration: $(notdir $@) ...
	@echo Building $(notdir $@) ...
	ndstool -c $(OUTPUT).nds -7 $(ARM7BIN) -9 $(ARM9BIN) $(ICON) "$(TITLE);$(SUBTITLE1);$(SUBTITLE2)"
	@echo Built $(notdir $@) .

$(CONF) : $(OUTPUT).nds
	@echo Built configuration: $(notdir $@) .

clean :
	@echo Cleaning releases ...
	@rm -f $(BUILDDIR)/*.d
	@rm -f $(BUILDDIR)/*.o
	@rm -f $(RELEASEDIR)/*.*ds*
	@echo Cleaned.

rebuild : all

