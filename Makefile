CFLAGS  = $(shell pkg-config --cflags libusb-1.0)
LDFLAGS = $(shell pkg-config --libs libusb-1.0)

PREFIX?=/usr/local

all: pl2303gpio

OBJS=usb.c main.c
pl2303gpio: $(OBJS)
	$(CC) $(CFLAGS) -Wall -Werror -I"../include" -o $(@) $(^) $(LDFLAGS) -lusb

clean:
	-rm pl2303gpio

install: pl2303gpio
	cp pl2303gpio $(PREFIX)/bin
	cp 10-pl2303userspace.rules /etc/udev/rules.d