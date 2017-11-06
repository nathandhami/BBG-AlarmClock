# Edit this file to compile extra C files into their own programs.
TARGET= lcd

SRC_DIR= src

CPP_SOURCES= main.cpp I2CIO.cpp LCD.cpp LiquidCrystal_I2C.cpp alarm.cpp udp.cpp
	   	     
C_SOURCES = utils.c deviceread.c keypad.c

OBJDIR = build

# lists of .o files
C_OBJ_LIST = $(addsuffix .o, $(basename $(C_SOURCES)))
CPP_OBJ_LIST = $(addsuffix .o, $(basename $(CPP_SOURCES)))

C_OBJ = $(addprefix $(OBJDIR)/, $(C_OBJ_LIST))
CPP_OBJ = $(addprefix $(OBJDIR)/, $(CPP_OBJ_LIST))

LFLAGS = -Llib -lpthread -lasound -static-libstdc++
IFLAGS = -Iincludes -L$(HOME)/cmpt433/public/asound_lib_BBB

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
NODEDIR = wakeUp-server

CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
CPPFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -std=c++11 -static-libstdc++


all: init lcd nodeserver

lcd: $(CPP_OBJ) $(C_OBJ)
	$(CC_CPP) $(CPPFLAGS) $(IFLAGS) $(CPP_OBJ) $(C_OBJ) $(SRC_DIR)/i2c-dev.h -o $(OUTDIR)/$(TARGET) $(LFLAGS)
	
init:
	  mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
				$(CC_CPP) $(CPPFLAGS) $(IFLAGS) -c $< -o $@ 

$(OBJDIR)/%.o: $(SRC_DIR)/%.c
				$(CC_C) $(CFLAGS) $(IFLAGS) -c  $< -o $@

nodeserver:
		mkdir -p $(PUBDIR)/$(NODEDIR)-copy
		cp -R $(NODEDIR)/* $(PUBDIR)/$(NODEDIR)-copy

clean:
	rm -f $(OUTDIR)/$(TARGET)
	rm -rf $(PUBDIR)/$(NODEDIR)-copy
	rm -rf $(OBJDIR)