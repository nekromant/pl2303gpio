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


void handle_error(int ret)
{
	if (ret<0) { 
		perror("Failed to write to PL2303 device");
		fprintf(stderr, "Have you installed the correct udev rules?\n");
		exit(1);
	}
}


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

static struct option long_options[] =
{
	/* These options set a flag. */
	{"help",    no_argument,       0, 'h'},
	{"gpio",    required_argument, 0, 'g'},
	{"in",      optional_argument, 0, 'i'},
	{"out",     required_argument, 0, 'o'},
	{"sleep",   required_argument, 0, 's'},
	{"read",    no_argument,       0, 'r'},
	{0, 0, 0, 0}
};
  
void usage(const char *self)
{
	printf("PL2303HXA userspace GPIO control tool\n"
	       "(c) Andrew 'Necromant' Andrianov 2014, License: GPLv3\n"
	       "Usage: %s [action1] [action2] ...\n"
	       "Options are: \n"
	       "\t -g/--gpio  n  - select GPIO, n=0, 1\n"
	       "\t -i/--in       - configure GPIO as input\n"
	       "\t -o/--out v    - configure GPIO as output with value v\n"
	       "\t -r/--read v   - Read current GPIO value\n\n"
	       "\t -s/--sleep v  - Delay for v ms\n\n"
	       "Examples: \n"
	       "\t%s --gpio=1 --out 1\n"
	       "\t%s --gpio=0 --out 0 --gpio=1 --in\n\n"
	       "All arguments are executed from left to right, you can add \n"
	       "delays using --sleep v option. e.g. \n"
	       "\t%s --gpio=0 --out 0 --sleep 1000 --gpio=0 --out 1\n"
	       "\n", self, self, self, self);
}

extern usb_dev_handle *nc_usb_open(int vendor, int product, char *vendor_name, char *product_name, char *serial);
void check_handle(usb_dev_handle **h)
{
	if (*h)
		return;

	/* TODO: Make a proper way to select different PL2303 devices. */
	*h = nc_usb_open(I_VENDOR_NUM, I_PRODUCT_NUM, NULL, NULL, NULL);
	if (!(*h)) {
		fprintf(stderr, "No PL2303 USB device %04x:%04x found ;(\n", I_VENDOR_NUM, I_PRODUCT_NUM);
		exit(1);
	}

	/* We don't set config or claim interface, pl2303 kernel driver does 
	 * that for us. 
	 */
}

int main(int argc, char* argv[])
{
	char c;
	usb_dev_handle *h = NULL;
	int gpio=0; 
	if (argc == 1) 
	{
		usage(argv[0]);
		exit(1);
	}
	while(1) {
		int option_index = 0;

		c = getopt_long (argc, argv, "hg:i:o:r:s:",
				 long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c)
		{
		case 'h':
			usage(argv[0]);
			exit(1);
			break;
		case 'g':
			gpio = atoi(optarg);
			break;
		case 'i':
		{
			int v=0; 
			check_handle(&h);
			if (optarg)
				v = atoi(optarg);
			gpio_in(h, gpio, v); 
			break;
		}
		case 'o':
		{
			int v=0; 
			check_handle(&h);
			if (optarg)
				v = atoi(optarg);
			gpio_out(h, gpio, v); 
			break;
		}
		case 's':
		{
			unsigned long n = atoi(optarg);
			usleep(n*1000);
			break;
		}
		case 'r': 
		{
			check_handle(&h);
			printf("%d\n", gpio_read(h, gpio) ? 1 : 0); 
			break;
		}

		}
	}
	return 0;
}
