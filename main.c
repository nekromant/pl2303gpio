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

int get_device_vid();
int get_device_pid();
void check_handle(libusb_device_handle **h, int vid, int pid, const char* manuf, const char* product, const char* serial);
int gpio_read(libusb_device_handle *h, int gpio);
void gpio_in(libusb_device_handle *h, int gpio, int pullup);
void gpio_out(libusb_device_handle *h, int gpio, int value);


void handle_error(int ret)
{
	if (ret<0) { 
		perror("Failed to write to PL2303 device");
		fprintf(stderr, "Have you installed the correct udev rules?\n");
		exit(1);
	}
}

static struct option long_options[] =
{
	/* These options set a flag. */
	{"help",    no_argument,             0, 'h'},
	{"gpio",    required_argument,       0, 'g'},
	{"in",      optional_argument,       0, 'i'},
	{"out",     required_argument,       0, 'o'},
	{"sleep",   required_argument,       0, 's'},
	{"read",    no_argument,             0, 'r'},
	{"product", required_argument,       0, 'p'},
	{"manuf",   required_argument,       0, 'm'},
	{"serial",  required_argument,       0, 'n'},
	{0, 0, 0, 0}
};
  
void usage(const char *self)
{
	printf("PL2303HXA/CP2103 userspace GPIO control tool\n"
	       "(c) Andrew 'Necromant' Andrianov 2014, License: GPLv3\n"
	       "Usage: %s [action1] [action2] ...\n"
	       "Options are: \n"
	       "\t --product=blah    - Use device with this 'product' string\n"
	       "\t --serial=blah     - Use device with this 'serial' string\n"
	       "\t --manuf=blah      - Use device with this 'manufacturer' string\n"
	       "\t -g/--gpio  n      - select GPIO, n=0, 1\n"
	       "\t -i/--in           - configure GPIO as input\n"
	       "\t -o/--out v        - configure GPIO as output with value v\n"
	       "\t -r/--read v       - Read current GPIO value\n\n"
	       "\t -s/--sleep v      - Delay for v ms\n\n"
	       "Examples: \n"
	       "\t%s --gpio=1 --out 1\n"
	       "\t%s --gpio=0 --out 0 --gpio=1 --in\n\n"
	       "All arguments are executed from left to right, you can add \n"
	       "delays using --sleep v option. e.g. \n"
	       "\t%s --gpio=0 --out 0 --sleep 1000 --gpio=0 --out 1\n"
	       "\n", self, self, self, self);
}

int main(int argc, char* argv[])
{
	int c;
	libusb_device_handle *h = NULL;
	int gpio=0; 
	const char *product = NULL; 
	const char *manuf = NULL;
	const char *serial = NULL;  
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
		case 'p':
			product = optarg;
			break;
		case 'm':
			manuf = optarg;
			break;
		case 'n':
			serial = optarg;
			break;
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
			check_handle(&h, get_device_vid(), get_device_pid(), manuf, product, serial);
			if (optarg)
				v = atoi(optarg);
			gpio_in(h, gpio, v); 
			break;
		}
		case 'o':
		{
			int v=0; 
			check_handle(&h, get_device_vid(), get_device_pid(), manuf, product, serial);
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
			check_handle(&h, get_device_vid(), get_device_pid(), manuf, product, serial);
			printf("%d\n", gpio_read(h, gpio) ? 1 : 0); 
			break;
		}

		}
	}
	return 0;
}
