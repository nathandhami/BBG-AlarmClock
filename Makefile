OUTFILE = alarmclock
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
SRC = main.c

	
all: app

app:
	$(CC_C) $(CFLAGS) $(SRC) -o $(OUTDIR)/$(OUTFILE)

help:
	@echo "Build skill-testing alarm clock program for Beaglebone green"
	@echo "Targets include all, clean"

clean:
	rm $(OUTDIR)/$(OUTFILE)
