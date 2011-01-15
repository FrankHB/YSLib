# DS Makefile for YShell by Franksoft 2009 - 2011
# CodePage = ANSI / GBK;
# CTime = 2009-11-12 21:26:30 + 08:00;
# UTime = 2011-01-15 14:41 + 08:00;
# Version = 0.3112;

# This makefile is written based on devkitPro example template.

#--------------------------------------------------------------------------------
.SUFFIXES:
#--------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TOPDIR		:=	$(CURDIR)

#--------------------------------------------------------------------------------

export TARGET		:=	$(shell basename $(CURDIR))
export BUILD		?=	release
export RELEASE		=	$(BUILD)
export GBUILD		=	$(TOPDIR)/$(BUILD)/$(TARGET)
export OUTPUT		=	$(TOPDIR)/$(RELEASE)/$(TARGET)

#--------------------------------------------------------------------------------

GAME_TITLE		:= YSTest
GAME_SUBTITLE1 	:= Franksoft
GAME_SUBTITLE2 	:= 2009 - 2011
GAME_ICON		:= $(TOPDIR)/common/data/logo.bmp

#--------------------------------------------------------------------------------

include $(DEVKITARM)/ds_rules

#--------------------------------------------------------------------------------
# If you're using EFS_lib, uncomment "USE_EFS = YES" here.
#--------------------------------------------------------------------------------

#USE_EFS = YES

PATH := $(DEVKITARM)/bin:$(PATH)
PROGNAME = YShell
OFILES +=
ADD_LIBS +=

#--------------------------------------------------------------------------------
# Be sure to change these default banner TEXTs. This is the name your project will
# display in the DS menu (including some flash cards). Create a custom LOGO.BMP
# icon for your project, too!
#--------------------------------------------------------------------------------

TITLE		:= $(GAME_TITLE)
SUBTITLE1 	:= $(GAME_SUBTITLE1)
SUBTITLE2 	:= $(GAME_SUBTITLE2)
ICON		:= -b $(GAME_ICON)

.PHONY: $(BUILD) clean rebuild $(GBUILD).arm7 $(GBUILD).arm9

#--------------------------------------------------------------------------------
# main targets
#--------------------------------------------------------------------------------
all : $(BUILD) $(OUTPUT).nds
#--------------------------------------------------------------------------------

%.ds.gba: %.nds
	@dsbuild $<
	@echo Built: $(notdir $(OUTPUT)).nds .
	@echo Built: $(notdir $@) .
#	@cp $(TOPDIR)/$(RELEASE)/$(notdir $@) $(TOPDIR)/$(RELEASE)/$(notdir $(OUTPUT)).sc.nds
#	@echo Built: $(notdir $(OUTPUT)).sc.nds .
ifeq ($(strip $(USE_EFS)), YES)
	@$(CURDIR)/../efs $(OUTPUT).nds
#	@$(CURDIR)/../efs $(OUTPUT).ds.gba
#	@$(CURDIR)/../efs $(OUTPUT).sc.nds
endif

#--------------------------------------------------------------------------------
$(OUTPUT).nds	:	$(GBUILD).arm7 $(GBUILD).arm9
ifeq ($(strip $(USE_EFS)), YES)
	ndstool -c $(OUTPUT).nds -7 $(GBUILD).arm7 -9 $(GBUILD).arm9 $(ICON) "$(TITLE);$(SUBTITLE1);$(SUBTITLE2)" -d $(TOPDIR)/efsroot
	$(TOPDIR)/efs.exe $(OUTPUT).nds
else
	ndstool -c $(OUTPUT).nds -7 $(GBUILD).arm7 -9 $(GBUILD).arm9 $(ICON) "$(TITLE);$(SUBTITLE1);$(SUBTITLE2)"
#	@cp $(OUTPUT).nds $(OUTPUT)_.nds
	@echo Patching DLDI GBA Movie Player [Compact Flash] ...
	@D:/Progra~2/DLDIrc/dlditool.exe "D:\Program Files (x86)\DLDIrc\drivers\mpcf.dldi" $(OUTPUT).nds
endif
	@echo Built $(notdir $@) .

#--------------------------------------------------------------------------------
$(GBUILD).arm7	: arm7/$(TARGET).elf
$(GBUILD).arm9	: arm9/$(TARGET).elf

#--------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#--------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#--------------------------------------------------------------------------------

$(BUILD):
# Before compiling, show any warning messages that might have detected
ifneq ($(strip $(TEXT_TO_SAY_1)),)
ifeq ($(strip $(MSG_IS_ERROR)),)
	@echo " ===================================="
	@echo "/ YShell Project Compilation Warning /"
	@echo "==================================== "
else
	@echo " =================================="
	@echo "/ YShell Project Compilation Error /"
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
	@[ -d $(TOPDIR)/$(RELEASE) ] || mkdir -p $(TOPDIR)/$(RELEASE)
	@[ -d $(TOPDIR)/$(BUILD) ] || mkdir -p $(TOPDIR)/$(BUILD)
	@[ -d $@ ] || mkdir -p $@
#	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
endif

#--------------------------------------------------------------------------------

clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	@echo Cleaning releases ...
	@rm -f $(TOPDIR)/$(BUILD)/*.d
	@rm -f $(TOPDIR)/$(BUILD)/*.o
	@rm -f $(TOPDIR)/$(RELEASE)/*.*arm*
	@rm -f $(TOPDIR)/$(RELEASE)/*.*ds*
	@echo Cleaned.

#--------------------------------------------------------------------------------

rebuild: all

