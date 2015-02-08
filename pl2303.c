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
#include <usb.h>
#include <getopt.h>


#define _GNU_SOURCE
#include <getopt.h>


#define I_VENDOR_NUM        0x67b
#define I_PRODUCT_NUM       0x2303


#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		0x01

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01



/* Get current GPIO register from PL2303 */
char gpio_read_reg(usb_dev_handle *h)
{
	char buf;
	int bytes = usb_control_msg(
		h,             // handle obtained with usb_open()
		VENDOR_READ_REQUEST_TYPE, // bRequestType
		VENDOR_READ_REQUEST,      // bRequest
		0x0081,              // wValue
		0,              // wIndex
		&buf,             // pointer to destination buffer
		1,  // wLength
		100
		);
	handle_error(bytes);
	return buf;
}

void gpio_write_reg(usb_dev_handle *h, unsigned char reg)
{
	int bytes = usb_control_msg(
		h,             // handle obtained with usb_open()
		VENDOR_WRITE_REQUEST_TYPE, // bRequestType
		VENDOR_WRITE_REQUEST,      // bRequest
		1,              // wValue
		reg,              // wIndex
		0,             // pointer to destination buffer
		0,  // wLength
		6000
		);
	handle_error(bytes);
	
}

int gpio_dir_shift(int gpio) {
	if (gpio == 0) 
		return 4;
	if (gpio == 1) 
		return 5;
	return 4; /* default to 0 */
}

int gpio_val_shift(int gpio) {
	if (gpio == 0) 
		return 6;
	if (gpio == 1) 
		return 7;
	return 6; /* default to 0 */
}


void gpio_out(usb_dev_handle *h, int gpio, int value)
{
	int shift_dir = gpio_dir_shift(gpio);
 	int shift_val = gpio_val_shift(gpio);
	unsigned char reg = gpio_read_reg(h);
	reg |= (1 << shift_dir);
	reg &= ~(1 << shift_val);
	reg |= (value << shift_val);
	gpio_write_reg(h, reg);
}

void gpio_in(usb_dev_handle *h, int gpio, int pullup)
{
	int shift_dir = gpio_dir_shift(gpio);
 	int shift_val = gpio_val_shift(gpio);

	unsigned char reg = gpio_read_reg(h);
	reg &= ~(1 << shift_dir);
	reg &= ~(1 << shift_val);
	reg |= (pullup << shift_val);
	gpio_write_reg(h, reg);
}

int gpio_read(usb_dev_handle *h, int gpio)
{
	unsigned char r = gpio_read_reg(h);
	int shift = gpio_val_shift(gpio);
	return (r & (1<<shift));
}

extern usb_dev_handle *nc_usb_open(int vendor, int product, char *vendor_name, char *product_name, char *serial);
void check_handle(usb_dev_handle **h, const char* manuf, const char* product, const char* serial)
{
	if (*h)
		return;

	*h = nc_usb_open(I_VENDOR_NUM, I_PRODUCT_NUM, manuf, product, serial);
	if (!(*h)) {
		fprintf(stderr, "No PL2303 USB device %04x:%04x found ;(\n", I_VENDOR_NUM, I_PRODUCT_NUM);
		exit(1);
	}
}
