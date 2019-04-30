
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libusb.h>




static int ncusb_match_string(libusb_device_handle *dev, int index, const char* string)
{
	unsigned char tmp[256];
	libusb_get_string_descriptor_ascii(dev, index, tmp, 256);
	if (string == NULL)
		return 1; /* NULL matches anything */
	return (strcmp(string, (char*) tmp)==0);
}



void ncusb_device_list(struct libusb_context *ctx,
					  int vendor, int product)
{
	libusb_init(NULL);
	libusb_device **list;
	ssize_t cnt = libusb_get_device_list(ctx, &list);
	ssize_t i = 0;

	if (cnt < 0)
		return;

	for(i = 0; i < cnt; i++) {
		int err = 0;
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		libusb_device_handle *handle;
		err = libusb_open(device, &handle);
		if (err)
			continue;

		int r = libusb_get_device_descriptor( device, &desc );
		if (r) {
			libusb_close(handle);
			continue;
		}

		if ( desc.idVendor == vendor && desc.idProduct == product) {
			unsigned char serial[256];
			unsigned char manuf[256];
			unsigned char product[256];		
			memset(serial, 0x0, sizeof(serial));		
			memset(manuf, 0x0, sizeof(manuf));		
			memset(product, 0x0, sizeof(product));					
			int port = libusb_get_port_number(device);
			libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, manuf, 256);
			libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial, 256);
			libusb_get_string_descriptor_ascii(handle, desc.iProduct, product, 256);
			printf("Port %d: Manuf: '%s' Product: '%s' Serial: '%s'\n", 
				port, manuf, serial, product);
		}

	}

}

struct libusb_device_handle *ncusb_find_and_open(struct libusb_context *ctx,
					  int vendor, int product,
					  const char *vendor_name,
					  const char *product_name,
					  const char *serial,
					  int port)
{
	libusb_device_handle *found = NULL;
	libusb_device **list;
	ssize_t cnt = libusb_get_device_list(ctx, &list);
	ssize_t i = 0;

	if (cnt < 0){
		return NULL;
	}

	for(i = 0; i < cnt; i++) {
		int err = 0;
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		libusb_device_handle *handle;
		err = libusb_open(device, &handle);
		if (err)
			continue;

		int dev_port = libusb_get_port_number(device);

		int r = libusb_get_device_descriptor( device, &desc );
		if (r) {
			libusb_close(handle);
			continue;
		}

		if ( desc.idVendor == vendor && desc.idProduct == product &&
		     ncusb_match_string(handle, desc.iManufacturer, vendor_name) &&
		     ncusb_match_string(handle, desc.iProduct,      product_name) &&
		     ncusb_match_string(handle, desc.iSerialNumber, serial) &&
			 ((port < 0) || port ==  dev_port)
			)
		{
			found = handle;
		}

		if (found)
			break;

	}
	libusb_free_device_list(list, 1);

	return found;
}


void check_handle(libusb_device_handle **h, int vid, int pid, const char* manuf, const char* product, const char* serial, int port)
{
	if (*h)
		return;
	libusb_init(NULL);
	*h = ncusb_find_and_open(NULL, vid, pid, manuf, product, serial, port);
	if (!(*h)) {
		fprintf(stderr, "No PL2303 USB device %04x:%04x found ;(\n", vid, pid);
		exit(1);
	}
}
