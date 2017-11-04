# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= lcd

SOURCES= main.cpp I2CIO.cpp LCD.cpp LiquidCrystal_I2C.cpp i2c-dev.h

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
CPPFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -std=c++11

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

# -pg for supporting gprof profiling.
#CFLAGS += -pg



all:
	$(CC_C) $(CFLAGS) $(LFLAGS) -o alarm.o alarm.c -lasound 
	$(CC_CPP) $(CPPFLAGS) $(SOURCES) alarm.o -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lasound  -lpthread

wave:
	mkdir -p $(OUTDIR)/wave-files/
	cp wave-files/* $(OUTDIR)/wave-files/ 

clean:
	rm -f $(OUTDIR)/$(TARGET)