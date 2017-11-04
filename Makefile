# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= lcd
SRC_DIR= src/
SOURCES= $(SRC_DIR)main.cpp $(SRC_DIR)I2CIO.cpp $(SRC_DIR)LCD.cpp $(SRC_DIR)LiquidCrystal_I2C.cpp \
	   	 $(SRC_DIR)i2c-dev.h $(SRC_DIR)utils.c

LFLAGS = -Llib -lpthread
IFLAGS = -Iincludes

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
CPPFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -std=c++11

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: lcd

wave:
	mkdir -p $(OUTDIR)/wave-files/
	cp wave-files/* $(OUTDIR)/wave-files/ 

lcd:
	$(CC_C) $(CFLAGS) $(LFLAGS) $(IFLAGS) -o alarm.o $(SRC_DIR)alarm.c -lasound 
	$(CC_CPP) $(CPPFLAGS) $(IFLAGS) $(SOURCES) alarm.o -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lasound  -lpthread
			
nodeserver:  


clean:
	rm -f $(OUTDIR)/$(TARGET)
