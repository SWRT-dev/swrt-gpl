// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/usb/core/otg_whitelist.h
 *
 * Copyright (C) 2004 Texas Instruments
 */

/*
 * This OTG and Embedded Host Whitelist is "Targeted Peripheral List".
 * It should mostly use of USB_DEVICE() or USB_DEVICE_VER() entries..
 *
 * YOU _SHOULD_ CHANGE THIS LIST TO MATCH YOUR PRODUCT AND ITS TESTING!
 */

static struct usb_device_id whitelist_table[] = {

/* hubs are optional in OTG, but very handy ... */
{ USB_DEVICE_INFO(USB_CLASS_HUB, 0, 0), },
{ USB_DEVICE_INFO(USB_CLASS_HUB, 0, 1), },

#ifdef	CONFIG_USB_PRINTER		/* ignoring nonstatic linkage! */
/* FIXME actually, printers are NOT supposed to use device classes;
 * they're supposed to use interface classes...
 */
{ USB_DEVICE_INFO(7, 1, 1) },
{ USB_DEVICE_INFO(7, 1, 2) },
{ USB_DEVICE_INFO(7, 1, 3) },
#endif

#ifdef	CONFIG_USB_NET_CDCETHER
/* Linux-USB CDC Ethernet gadget */
{ USB_DEVICE(0x0525, 0xa4a1), },
/* Linux-USB CDC Ethernet + RNDIS gadget */
{ USB_DEVICE(0x0525, 0xa4a2), },
#endif

#if	IS_ENABLED(CONFIG_USB_TEST)
/* gadget zero, for testing */
{ USB_DEVICE(0x0525, 0xa4a0), },
#endif

/* xhci-mtk usb3 root-hub */
{ USB_DEVICE(0x1d6b, 0x0003), },

/* xhci-mtk usb2 root-hub */
{ USB_DEVICE(0x1d6b, 0x0002), },

/*  */
{ USB_INTERFACE_INFO(USB_CLASS_MASS_STORAGE, 0, 0) },

{ }	/* Terminating entry */
};

static bool usb_match_any_interface(struct usb_device *udev,
				    const struct usb_device_id *id)
{
	unsigned int i;

	for (i = 0; i < udev->descriptor.bNumConfigurations; ++i) {
		struct usb_host_config *cfg = &udev->config[i];
		unsigned int j;

		for (j = 0; j < cfg->desc.bNumInterfaces; ++j) {
			struct usb_interface_cache *cache;
			struct usb_host_interface *intf;

			cache = cfg->intf_cache[j];
			if (cache->num_altsetting == 0)
				continue;

			intf = &cache->altsetting[0];
			if (id->bInterfaceClass == intf->desc.bInterfaceClass)
				return true;
		}
	}

	return false;
}

static int is_targeted(struct usb_device *dev)
{
	struct usb_device_id	*id = whitelist_table;

	/* HNP test device is _never_ targeted (see OTG spec 6.6.6) */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
	     le16_to_cpu(dev->descriptor.idProduct) == 0xbadd))
		return 0;

	/* OTG PET device is always targeted (see OTG 2.0 ECN 6.4.2) */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
	     le16_to_cpu(dev->descriptor.idProduct) == 0x0200))
		return 1;

	/* NOTE: can't use usb_match_id() since interface caches
	 * aren't set up yet. this is cut/paste from that code.
	 */
	for (id = whitelist_table; id->match_flags; id++) {
		if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) &&
		    id->idVendor != le16_to_cpu(dev->descriptor.idVendor))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) &&
		    id->idProduct != le16_to_cpu(dev->descriptor.idProduct))
			continue;

		/* No need to test id->bcdDevice_lo != 0, since 0 is never
		   greater than any unsigned number. */
		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) &&
		    (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) &&
		    (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) &&
		    (id->bDeviceClass != dev->descriptor.bDeviceClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) &&
		    (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) &&
		    (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_INT_INFO) &&
		    !usb_match_any_interface(dev, id))
			continue;

		return 1;
	}

	/* add other match criteria here ... */


	/* OTG MESSAGE: report errors here, customize to match your product */
	dev_err(&dev->dev, "device v%04x p%04x is not supported\n",
		le16_to_cpu(dev->descriptor.idVendor),
		le16_to_cpu(dev->descriptor.idProduct));

	return 0;
}

