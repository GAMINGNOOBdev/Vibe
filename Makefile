TARGET = Vibe

INCDIR = inc lib gu2gl minimp3 libwave/include
SRCDIR = src

CFILES := $(shell find -L $(SRCDIR) -type f -name '*.c')
CFILES := $(subst src/,,$(CFILES))
OBJS = $(addprefix src/,$(CFILES:.c=.o))

OBJS += \
		lib/stb_vorbis.o \
		libwave/src/wave.o

CFLAGS = -DTINYWAV_USE_MALLOC -D__GLIBC_USE\(...\)=0

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
