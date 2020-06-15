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
#include <libusb.h>
#include <getopt.h>


#define _GNU_SOURCE
#include <getopt.h>


#define I_VENDOR_NUM        0x67b
#define I_PRODUCT_NUM       0x23a3


#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		0x81

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x80

struct pl2303_pinctrl{
	uint16_t reg_read_mask;

	uint16_t dir_reg;
	uint8_t dir_mask; //out mode: set with mask. input mode: clean with mask

	uint16_t in_reg;
	uint8_t in_mask;

	uint16_t out_reg;
	uint8_t out_mask;
};


struct pl2303_pinctrl pins[] = {
	{0x80, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01},
	{0x80, 0x03, 0x02, 0x01, 0x02, 0x01, 0x02},
	{0x80, 0x03, 0x04, 0x01, 0x04, 0x01, 0x04},
	{0x80, 0x03, 0x08, 0x01, 0x08, 0x01, 0x08},
};

#define PINS_MAX	(sizeof(pins)/sizeof(*pins))

//internal api
unsigned char pl2303_read_reg(libusb_device_handle *h, unsigned short reg)
{
	unsigned char value;
	int bytes = libusb_control_transfer(
		h,             // handle obtained with usb_open()
		VENDOR_READ_REQUEST_TYPE, // bRequestType
		VENDOR_READ_REQUEST,      // bRequest
		reg,           // wValue
		0,             // wIndex
		&value,        // pointer to destination buffer
		1,             // wLength
		1000
		);
	handle_error(bytes);
	//fprintf(stderr, "read reg[0x%02x] = 0x%02x\n", reg ,value);
	return value;
}


void pl2303_write_reg(libusb_device_handle *h, unsigned short reg, unsigned char value)
{
	int bytes = libusb_control_transfer(
		h,             // handle obtained with usb_open()
		VENDOR_WRITE_REQUEST_TYPE, // bRequestType
		VENDOR_WRITE_REQUEST,      // bRequest
		reg,           // wValue
		value,         // wIndex
		0,             // pointer to destination buffer
		0,             // wLength
		1000
		);
	//fprintf(stderr, write reg[0x%02x] = 0x%02x\n", reg ,value);
	handle_error(bytes);
}


//export API
int get_device_vid()
{
	return I_VENDOR_NUM;
}

int get_device_pid()
{
	return I_PRODUCT_NUM;
}

void gpio_out(libusb_device_handle *h, int gpio, int value)
{
	unsigned char reg_value;

	if(0 <= gpio && gpio < PINS_MAX){
		//direct
		reg_value = pl2303_read_reg(h, pins[gpio].dir_reg | pins[gpio].reg_read_mask);
		reg_value |= pins[gpio].dir_mask;
		pl2303_write_reg(h, pins[gpio].dir_reg, reg_value);

		//out
		reg_value = pl2303_read_reg(h, pins[gpio].out_reg | pins[gpio].reg_read_mask);

		if(!!value){
			reg_value |= pins[gpio].out_mask;
		} else {
			reg_value &= ~pins[gpio].out_mask;
		}
		pl2303_write_reg(h, pins[gpio].out_reg, reg_value);
	} else {
		fprintf(stderr, "Error: gpio%d is not support\n", gpio);
		exit(1);
	}
}

void gpio_in(libusb_device_handle *h, int gpio, int pullup)
{
	unsigned char reg_value;

	if(0 <= gpio && gpio < PINS_MAX){
		//direct
		reg_value = pl2303_read_reg(h, pins[gpio].dir_reg | pins[gpio].reg_read_mask);
		reg_value &= ~pins[gpio].dir_mask;
		pl2303_write_reg(h, pins[gpio].dir_reg, reg_value);
	} else {
		fprintf(stderr, "Error: gpio%d is not support\n", gpio);
		exit(1);
	}
}

int gpio_read(libusb_device_handle *h, int gpio)
{
	unsigned char reg_value;

	if(0 <= gpio && gpio < PINS_MAX){
		//in
		reg_value = pl2303_read_reg(h, pins[gpio].in_reg | pins[gpio].reg_read_mask);
		return !!(reg_value &= pins[gpio].in_mask);
	} else {
		fprintf(stderr, "Error: gpio%d is not support\n", gpio);
		exit(1);
	}
}
