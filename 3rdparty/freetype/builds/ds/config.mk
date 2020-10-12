DELETE    := rm -fr
CAT       := cat
SEP       := /
BUILD_DIR := $(TOP_DIR)/builds/arm-eabi
PLATFORM  := arm-eabi

LIB_DIR := $(OBJ_DIR)
LIBRARY := lib$(PROJECT)
PREFIX := $(DEVKITARM)/bin/arm-none-eabi-

CC := $(PREFIX)gcc
AR := $(PREFIX)ar
COMPILER_SEP := /
O  := o
SO := o
A  := a
SA := a
I := -I
D := -D
L := -l
T := -o$(space)
CPPFLAGS += -DFT_CONFIG_MODULES_H="<ftmodule.h>"
CFLAGS ?= -c -Os -Wall -fdata-sections -ffunction-sections \
	-DFT_CONFIG_OPTION_OLD_INTERNALS -DNDEBUG
ANSIFLAGS :=
CLEAN_LIBRARY  = $(DELETE) $(OBJ_DIR)/*
LINK_LIBRARY   = $(AR) -r $@ $(OBJECTS_LIST)

include $(TOP_DIR)/builds/freetype.mk

$(PROJECT_LIBRARY): $(OBJECTS_LIST)
	  $(LINK_LIBRARY)

