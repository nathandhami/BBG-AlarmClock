# Edit this file to compile extra C files into their own programs.
TARGET= wakeup-app
SRC_DIR= src/
SOURCES= $(SRC_DIR)main.cpp $(SRC_DIR)I2CIO.cpp $(SRC_DIR)LCD.cpp $(SRC_DIR)LiquidCrystal_I2C.cpp \
	   	 $(SRC_DIR)i2c-dev.h $(SRC_DIR)utils.c $(SRC_DIR)udp.cpp

LFLAGS = -Llib -lpthread
IFLAGS = -Iincludes

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
NODEDIR = wakeUp-server
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -std=c++11

# -pg for supporting gprof profiling.
#CFLAGS += -pg

all: app nodeserver
	
app:
	$(CC_CPP) $(CFLAGS) $(IFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) $(LFLAGS)
			
nodeserver:
		mkdir -p $(PUBDIR)/$(NODEDIR)-copy
		cp -R $(NODEDIR)/* $(PUBDIR)/$(NODEDIR)-copy

clean:
	rm -f $(OUTDIR)/$(TARGET)
	rm -rf $(PUBDIR)/$(NODEDIR)-copy
