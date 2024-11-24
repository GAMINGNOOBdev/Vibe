TARGET = Vibe

INCDIR = inc lib gu2gl minimp3 libwave/include
SRCDIR = src

SOURCE_FILES_FOR_COMPILING = $(call recursiveWildCard,$(SRCDIR),*.c)

OBJS = \
		src/audio.o \
		src/beatmapList.o \
		src/callback.o \
		src/fileutil.o \
		src/fontFile.o \
		src/fontRenderer.o \
		src/gfx.o \
		src/input.o \
		src/logging.o \
		src/main.o \
		src/mainMenu.o \
		src/mesh.o \
		src/songlist.o \
		src/songSelect.o \
		src/sprite.o \
		src/strutil.o \
		src/texture.o \
		src/tilemap.o \
		src/time.o \
		src/vertex.o \
		lib/stb_vorbis.o \
		libwave/src/wave.o

CFLAGS = -DTINYWAV_USE_MALLOC
CXXFLAGS = $(CFLAGS) -std=c++14 -fno-rtti -fno-exceptions -Wall -Wno-narrowing
ASFLAGS = $(CFLAGS)

LIBS +=

include make.config

BUILD_PRX = 1
#PSP_FW_VERSION = 371
#PSP_FW_VERSION = 500
PSP_FW_VERSION = 660
#PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = $(TARGET)
PSP_EBOOT_ICON = Assets/LOGO.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
