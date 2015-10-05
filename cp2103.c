#include <stdio.h>
#include <stdlib.h>
/* According to POSIX.1-2001 */
#include <sys/select.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <stdint.h>
#include <libusb.h>

#define _GNU_SOURCE
#include <getopt.h>


#define I_VENDOR_NUM        0x10c4
#define I_PRODUCT_NUM       0xea60


#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		0x01

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01

/* CP2103 GPIO */
#define GPIO_0	0x01
#define GPIO_1	0x02
#define GPIO_2	0x04
#define GPIO_3	0x08
#define GPIO_MASK	(GPIO_0|GPIO_1|GPIO_2|GPIO_3)




int get_device_vid()
{
	return I_VENDOR_NUM;
}

int get_device_pid()
{
	return I_PRODUCT_NUM;
}

//GPIO_READ
//ret = cp210x_ctlmsg(port, 0xff, 0xc0, 0x00c2, 0, gpio, 1);
//type c0
//value c2
//index 0
//buffer 1 bytes
//length 1


/* Get current GPIO register from PL2303 */
char gpio_read_reg(libusb_device_handle *h)
{
	char buf;
	int bytes = libusb_control_transfer(
		h,             // handle obtained with usb_open()
		0xc0, // bRequestType
		0xff,      // bRequest
		0xc2,              // wValue
		0,              // wIndex
		&buf,             // pointer to destination buffer
		1,  // wLength
		100
		);
	handle_error(bytes);
	return buf;
}

// GPIO_WRITE
//request 0xff
//type 0x40
//value 0x37e1
//index gpioreg
//data NULL
//len 0
void gpio_write_reg(libusb_device_handle *h, uint16_t reg)
{
	int bytes = libusb_control_transfer(
		h,             // handle obtained with usb_open()
		0x40, // bRequestType
		0xff,      // bRequest
		0x37e1,              // wValue
		reg,              // wIndex
		0,             // pointer to destination buffer
		0,  // wLength
		1000
		);
	handle_error(bytes);
}


void gpio_out(libusb_device_handle *h, int gnum, int value)
{

	uint16_t gpio = 0;
	switch (gnum) { 
	case 0: 
		gpio |= GPIO_0;
		if (value) 
			gpio |= (GPIO_0 << 8);
		break;
	case 1: 
		gpio |= GPIO_1;
		if (value) 
			gpio |= (GPIO_1 << 8);
		break;
	case 2: 
		gpio |= GPIO_2;
		if (value) 
			gpio |= (GPIO_2 << 8);
		break;
	case 3: 
		gpio |= GPIO_3;
		if (value) 
			gpio |= (GPIO_3 << 8);
		break;
	}
	gpio_write_reg(h, gpio);
}

void gpio_in(libusb_device_handle *h, int gpio, int pullup)
{
	printf("FixMe: don't know how to make pins input on cp2103\n");
}

int gpio_read(libusb_device_handle *h, int gpio)
{
	printf("FixMe: don't know how to read pins on cp2103\n");
}

