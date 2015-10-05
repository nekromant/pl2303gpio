
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


struct libusb_device_handle *ncusb_find_and_open(struct libusb_context *ctx,
					  int vendor, int product,
					  const char *vendor_name,
					  const char *product_name,
					  const char *serial)
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

		int r = libusb_get_device_descriptor( device, &desc );
		if (r) {
			libusb_close(handle);
			continue;
		}

		if ( desc.idVendor == vendor && desc.idProduct == product &&
		     ncusb_match_string(handle, desc.iManufacturer, vendor_name) &&
		     ncusb_match_string(handle, desc.iProduct,      product_name) &&
		     ncusb_match_string(handle, desc.iSerialNumber, serial)
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


void check_handle(libusb_device_handle **h, int vid, int pid, const char* manuf, const char* product, const char* serial)
{
	if (*h)
		return;
	libusb_init(NULL);
	*h = ncusb_find_and_open(NULL, vid, pid, manuf, product, serial);
	if (!(*h)) {
		fprintf(stderr, "No PL2303 USB device %04x:%04x found ;(\n", vid, pid);
		exit(1);
	}
}
