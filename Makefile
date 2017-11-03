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

CFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -std=c++11

# -pg for supporting gprof profiling.
#CFLAGS += -pg



all:
	$(CC_CPP) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread


clean:
	rm -f $(OUTDIR)/$(TARGET)