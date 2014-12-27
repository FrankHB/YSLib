# Linux makefile for FreeImage

include Makefile.srcs

# General configuration variables:
DESTDIR ?= /
INCDIR ?= $(DESTDIR)/usr/include
INSTALLDIR ?= $(DESTDIR)/usr/lib

# Converts cr/lf to just lf
DOS2UNIX = dos2unix

LIBRARIES = -lstdc++

NASM ?= nasm
C_CXXFLAGS_GC ?= -fdata-sections -ffunction-sections
C_CXXFLAGS ?= -O3 -fPIC -pipe -Wno-attributes -fexceptions $(C_CXXFLAGS_GC)
CFLAGS ?= $(C_CXXFLAGS)
# OpenJPEG
CFLAGS += -DOPJ_STATIC
# LibRaw
CFLAGS += -DNO_LCMS
# LibJXR
CFLAGS += -DDISABLE_PERF_MEASUREMENT -D__ANSI__
CFLAGS += $(INCLUDE)
CXXFLAGS ?= -std=c++11 $(C_CXX_FLAGS) -Wno-ctor-dtor-privacy -DNDEBUG
# LibJXR
CXXFLAGS += -D__ANSI__
CXXFLAGS += $(INCLUDE)

ifeq ($(shell sh -c 'uname -m 2>/dev/null || echo not'),x86_64)
	CFLAGS += -fPIC
	CXXFLAGS += -fPIC
	NAFLAGS := -felf64 -DELF -D__x86_64__
	MODULES := $(SRCS) $(libsimd_64_SOURCES)
else ifeq ($(shell sh -c 'uname -m 2>/dev/null || echo not'),i686)
	NAFLAGS := -felf -DELF
	MODULES := $(SRCS) $(libsimd_SOURCES)
else
	$(error "Unsupported architecture found.")
endif

MODULES := $(MODULES:.asm=.asm.o)
MODULES := $(MODULES:.c=.c.o)
MODULES := $(MODULES:.cpp=.cpp.o)

TARGET  = freeimage
STATICLIB = lib$(TARGET).a
SHAREDLIB = lib$(TARGET)-$(VER_MAJOR).$(VER_MINOR).so
LIBNAME	= lib$(TARGET).so
VERLIBNAME = $(LIBNAME).$(VER_MAJOR)
HEADER = Source/FreeImage.h


default : all

all : dist

dist : FreeImage
	cp *.a Dist
	cp *.so Dist
	cp Source/FreeImage.h Dist

dos2unix:
	@$(DOS2UNIX) $(SRCS) $(INCLS)

FreeImage : $(STATICLIB) $(SHAREDLIB)

%.c.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.cpp.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.asm.o : %.asm ./Source/LibJPEG/simd/jsimdcfg.inc
	$(NASM) $(NAFLAGS) -I./Source/LibJPEG/simd/ $< -o $@

./Source/LibJPEG/simd/jsimdcfg.inc:
	gcc -E -I$. -I$./Source/LibJPEG/simd/ $./jsimdcfg.inc.h | grep -e "^[\;%]|^\ %" | sed 's%_cpp_protection_%%' | sed 's@% define@%define@g' > $@

$(STATICLIB) : $(MODULES)
	$(AR) r $@ $(MODULES)

$(SHAREDLIB) : $(MODULES)
	$(CC) -fPIC -s -shared -Wl,-soname,$(VERLIBNAME) $(LDFLAGS) -o $@ $(MODULES) $(LIBRARIES)

install :
	install -d $(INCDIR) $(INSTALLDIR)
	install -m 644 -o root -g root $(HEADER) $(INCDIR)
	install -m 644 -o root -g root $(STATICLIB) $(INSTALLDIR)
	install -m 755 -o root -g root $(SHAREDLIB) $(INSTALLDIR)
	ln -sf $(SHAREDLIB) $(INSTALLDIR)/$(VERLIBNAME)
	ln -sf $(VERLIBNAME) $(INSTALLDIR)/$(LIBNAME)	
#	ldconfig

clean :
	rm -f core Dist/*.* u2dtmp* ./Source/LibJPEG/simd/jsimdcfg.inc $(MODULES) $(STATICLIB) $(SHAREDLIB) $(LIBNAME)

