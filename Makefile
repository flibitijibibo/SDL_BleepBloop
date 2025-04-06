# Makefile for SDL_BleepBloop
# Written by Ethan "flibitijibibo" Lee

# Detect cross targets
TRIPLET=$(shell $(CC) -dumpmachine)
WINDOWS_TARGET=0
APPLE_TARGET=0
ifeq ($(OS), Windows_NT) # cygwin/msys2
	WINDOWS_TARGET=1
endif
ifneq (,$(findstring w64-mingw32,$(TRIPLET)))
	WINDOWS_TARGET=1
endif
ifneq (,$(findstring w64-windows,$(TRIPLET)))
	WINDOWS_TARGET=1
endif
ifneq (,$(findstring apple-darwin,$(TRIPLET)))
	APPLE_TARGET=1
endif

# Compiler
ifeq ($(WINDOWS_TARGET),1)
	TARGET = dll
	LDFLAGS += -static-libgcc
	EPICLIB = SDK/Bin/EOSSDK-Win64-Shipping.dll
else ifeq ($(APPLE_TARGET),1)
	CC += -mmacosx-version-min=11.0
	TARGET = dylib
	PREFIX = lib
	CFLAGS += -fpic -fPIC
	LDFLAGS += -install_name @rpath/libSDL_BleepBloop.dylib
	EPICLIB = SDK/Bin/libEOSSDK-Mac-Shipping.dylib
else
	TARGET = so
	PREFIX = lib
	CFLAGS += -fpic -fPIC
	EPICLIB = SDK/Bin/libEOSSDK-Linux-Shipping.so
endif

LIB = $(PREFIX)SDL_BleepBloop.$(TARGET)

CFLAGS += -O3

# Targets
steam:
	$(CC) $(CFLAGS) -shared -o $(LIB) src/BleepBloop_Steam.c -Iinclude -lSDL3 -DBLEEPBLOOP_STEAM
epic:
	$(CC) $(CFLAGS) -shared -o $(LIB) src/BleepBloop_Epic.c -Iinclude -ISDK/Include -lSDL3 $(EPICLIB) -DBLEEPBLOOP_EPIC
dummy:
	$(CC) $(CFLAGS) -shared -o $(LIB) src/BleepBloop_Dummy.c -Iinclude -lSDL3 -DBLEEPBLOOP_DUMMY
clean:
	rm -f $(LIB)
