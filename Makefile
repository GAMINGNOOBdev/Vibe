TARGET = Vibe

INCDIR = inc lib gu2gl
SRCDIR = src

SOURCE_FILES_FOR_COMPILING = $(call recursiveWildCard,$(SRCDIR),*.c)

OBJS = \
		src/audio.o \
		src/callback.o \
		src/gfx.o \
		src/input.o \
		src/logging.o \
		src/main.o \
		src/mainMenu.o \
		src/time.o \
		lib/stb_vorbis.o

CFLAGS =
CXXFLAGS = $(CFLAGS) -std=c++14 -fno-rtti -fno-exceptions -Wall -Wno-narrowing
ASFLAGS = $(CFLAGS)

LIBS +=

include make.config

BUILD_PRX = 1
PSP_FW_VERSION = 371
#PSP_FW_VERSION = 500
#PSP_FW_VERSION = 660
PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = $(TARGET)
PSP_EBOOT_ICON = Assets/LOGO.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
