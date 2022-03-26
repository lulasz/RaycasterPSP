TARGET = raycaster
OBJS = ./src/main.o ./src/common/callbacks.o ./src/common/helpers.o ./lib/glib2d.o 

INCDIR =
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

LIBDIR =
LIBS =
LDFLAGS	=

LIBS = -lpng -ljpeg -lz -lpspgu -lm -lpspvram -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Raycaster
PSP_EBOOT_ICON= ./res/gfx/eboot/ICON0.png
PSP_EBOOT_PIC1= ./res/gfx/eboot/PIC1.png
#PSP_EBOOT_SND0= ./src/gfx/SND0.at3

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
