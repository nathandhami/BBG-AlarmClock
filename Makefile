OUTFILE = alarmclock

OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
SRC = main.c alarm.c

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB
	
all: app

app:
	$(CC_C) $(CFLAGS) $(SRC) -o $(OUTDIR)/$(OUTFILE)  $(LFLAGS) -lpthread -lasound

wave:
	mkdir -p $(OUTDIR)/wave-files/
	cp wave-files/* $(OUTDIR)/wave-files/ 

help:
	@echo "Build skill-testing alarm clock program for Beaglebone green"
	@echo "Targets include all, clean"

clean:
	rm $(OUTDIR)/$(OUTFILE)
