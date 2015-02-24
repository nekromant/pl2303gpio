CFLAGS  = $(shell pkg-config --cflags libusb)
LDFLAGS = $(shell pkg-config --libs libusb)

PREFIX?=/usr/local

all: pl2303gpio cp2103gpio

OBJS=usb.c main.c
pl2303gpio: $(OBJS) pl2303.o
	$(CC) $(CFLAGS) -Wall -Werror -I"../include" -o $(@) $(^) $(LDFLAGS)

cp2103gpio: $(OBJS) cp2103.o
	$(CC) $(CFLAGS) -Wall -Werror -I"../include" -o $(@) $(^) $(LDFLAGS)

clean:
	-rm pl2303gpio cp2103gpio

install: pl2303gpio cp2103gpio
	cp pl2303gpio $(PREFIX)/bin
	cp cp2103gpio $(PREFIX)/bin

install-rules:	
	cp 10-pl2303_cp210x_userspace.rules /etc/udev/rules.d
	udevadm control --reload-rules

install-scripts:
	cp extra/serverctl   $(PREFIX)/bin/
	cp extra/serverd.lua $(PREFIX)/bin/
	cp etc/serverd.conf  $(PREFIX)/etc/
