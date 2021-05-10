CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror 
OUTDIR = $(HOME)/cmpt433/public/myApps
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: beatbox wav node

beatbox:
	$(CC_C) $(CFLAGS) main.c joystick.c beatbox.c audioMixer.c accelerometer.c udpHandler.c commonUtil.c -o ${OUTDIR}/beatbox $(LFLAGS) -lm -lpthread -lasound


wav:
	mkdir -p $(OUTDIR)/beatbox-wav-files/
	cp -R beatbox-wav-files/* $(OUTDIR)/beatbox-wav-files/

node:
	mkdir -p $(OUTDIR)/beatbox-server-copy/
	cp -R as3-server/* $(OUTDIR)/beatbox-server-copy/
	cd $(OUTDIR)/beatbox-server-copy/ && npm install

	
