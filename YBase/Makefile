# DS Makefile for YBase -> ARM9 by Franksoft 2009 - 2012
# CodePage = ANSI / GBK;
# CTime = 2009-12-18 12:27:40 +0800;
# UTime = 2012-03-21 16:19 +0800;
# Version = r3911;

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
# BUILD is the directory where object files and intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o
# GRAPHICS is a list of directories containing image files to be converted with grit
#----

export TARGET		:= $(shell basename $(CURDIR))
export BUILD		?= release
export RELEASE		= $(BUILD)
export SOURCES		:= source source/ystdex data
export INCLUDES		:= include data
export DATA			:= data

#----
# options for code generation
#----
ifeq ($(PLATFORM),DS)

# the prefix on the compiler executables
PREFIX	:=	arm-eabi-

ARCH_AS	:=	-mthumb -mthumb-interwork -march=armv5te
ARCH	:=	$(ARCH_AS) -mtune=arm946e-s

# NOTE: arm9tdmi isn't the correct CPU arch, but anything newer and LD
# *insists* it has a FPU or VFP, and it won't take no for an answer!

ASFLAGS	:=	$(ARCH) -mfpu=softfpa -mfpu=softvfp

CFLAGS	:=	-Wall \
 			-fomit-frame-pointer \
 			-ffast-math \
 			$(ARCH)

CFLAGS	+=	$(INCLUDE) -pedantic -DARM9
LDFLAGS :=	-specs=ds_arm9.specs $(ARCH) -mno-fpu -Wl,-Map,$(notdir $*.map)

ifneq ($(BUILD),debug)
CFLAGS	+=	-O3 -DNDEBUG
else
ASFLAGS	+=	-g
CFLAGS	+=	-g -O0 -DDEBUG
LDFLAGS	+=	-g
endif

CXXFLAGS	:=	$(CFLAGS) -fno-threadsafe-statics -fuse-cxa-atexit -std=c++0x \
 			-Wctor-dtor-privacy -Wnoexcept -Wold-style-cast -Wsign-promo

endif

#----
# path to tools - this can be deleted if you set the path in windows
#----
# export PATH		:=	/d/dev/ds/devkitARM_r11/bin:/bin

#----
# any extra libraries we wish to link with the project
#----
LIBS	:=	#-lnds9 -lfat

#----
# list of directories containing libraries, this must be the top level containing
# include and lib
#----
LIBDIRS	:=	#$(LIBNDS)

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
PCXFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.pcx)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

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

PNGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.png)))
PALFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.pal)))
RAWFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.raw)))
MAPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.map)))
JPEGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.jpg)))
MODFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.mod)))
GIFFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.gif)))
BMPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bmp)))
MP3FILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.mp3)))

#----
# use CXX for linking C++ projects, CC for standard C
#----
ifeq ($(strip $(CPPFILES)),)
#----
 	export LD	:=	$(CC)
#----
else
#----
 	export LD	:=	$(CXX)
#----
endif
#----

#export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
#					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES	:=	$(MAPFILES:.map=.o) $(RAWFILES:.raw=.o) $(PALFILES:.pal=.o) $(BINFILES:.bin=.o) $(PNGFILES:.png=.o) $(PCXFILES:.pcx=.o) $(JPEGFILES:.jpg=.o) $(MODFILES:.mod=.o) $(GIFFILES:.gif=.o) $(BMPFILES:.bmp=.o) $(MP3FILES:.mp3=.o)\
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include/nds) \
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
	@echo Cleaning $(TARGET) - ARM9 ...
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

#----
%.o : %.cpp
	@echo $(notdir $<)
	@$(CXX) -MM $(CXXFLAGS) -o $*.d $<
	@$(CXX) $(CXXFLAGS) -c $< -o$@

#----
%.o : %.c
	@echo $(notdir $<)
	@$(CC) -MM $(CFLAGS) -o $*.d $<
	@$(CC)  $(CFLAGS) -c $< -o$@

#----
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

#----
%.o	:	%.mp3
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.pcx
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.bin
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.png
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.raw
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.pal
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.map
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.mdl
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.jpg
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.mod
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.gif
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
%.o	:	%.bmp
#----
	@echo $(notdir $<)
	@$(bin2o)

#----
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#----
%.s %.h   : %.png %.grit
#----
	grit $< -fts -o$*

-include $(DEPSDIR)/*.d

#----
endif
#----

