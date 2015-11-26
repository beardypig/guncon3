/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>

static unsigned long debug = 0;
module_param(debug, ulong, 0444);
MODULE_PARM_DESC(debug, "Enable Debugging");

/*
 * Version Information
 */
#define DRIVER_VERSION "v0.2.0"
#define DRIVER_AUTHOR "Beardypig <guncon@beardypig.com>"
#define DRIVER_DESC "USB GunCon3 Driver"
#define DRIVER_LICENSE "GPL"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

static const unsigned char KEY_TABLE[320] = {
        0x75, 0xC3, 0x10, 0x31, 0xB5, 0xD3, 0x69, 0x84, 0x89, 0xBA, 0xD6, 0x89, 0xBD, 0x70, 0x19, 0x8E, 0x58, 0xA8,
        0x3D, 0x9B, 0x5D, 0xF0, 0x49, 0xE8, 0xAD, 0x9D, 0x7A, 0xD, 0x7E, 0x24, 0xDA, 0xFC, 0xD, 0x14, 0xC5, 0x23,
        0x91, 0x11, 0xF5, 0xC0, 0x4B, 0xCD, 0x44, 0x1C, 0xC5, 0x21, 0xDF, 0x61, 0x54, 0xED, 0xA2, 0x81, 0xB7, 0xE5,
        0x74, 0x94, 0xB0, 0x47, 0xEE, 0xF1, 0xA5, 0xBB, 0x21, 0xC8, 0x91, 0xFD, 0x4C, 0x8B, 0x20, 0xC1, 0x7C, 9, 0x58,
        0x14, 0xF6, 0, 0x52, 0x55, 0xBF, 0x41, 0x75, 0xC0, 0x13, 0x30, 0xB5, 0xD0, 0x69, 0x85, 0x89, 0xBB, 0xD6, 0x88,
        0xBC, 0x73, 0x18, 0x8D, 0x58, 0xAB, 0x3D, 0x98, 0x5C, 0xF2, 0x48, 0xE9, 0xAC, 0x9F, 0x7A, 0xC, 0x7C, 0x25, 0xD8,
        0xFF, 0xDC, 0x7D, 8, 0xDB, 0xBC, 0x18, 0x8C, 0x1D, 0xD6, 0x3C, 0x35, 0xE1, 0x2C, 0x14, 0x8E, 0x64, 0x83, 0x39,
        0xB0, 0xE4, 0x4E, 0xF7, 0x51, 0x7B, 0xA8, 0x13, 0xAC, 0xE9, 0x43, 0xC0, 8, 0x25, 0xE, 0x15, 0xC4, 0x20, 0x93,
        0x13, 0xF5, 0xC3, 0x48, 0xCC, 0x47, 0x1C, 0xC5, 0x20, 0xDE, 0x60, 0x55, 0xEE, 0xA0, 0x40, 0xB4, 0xE7, 0x74,
        0x95, 0xB0, 0x46, 0xEC, 0xF0, 0xA5, 0xB8, 0x23, 0xC8, 4, 6, 0xFC, 0x28, 0xCB, 0xF8, 0x17, 0x2C, 0x25, 0x1C,
        0xCB, 0x18, 0xE3, 0x6C, 0x80, 0x85, 0xDD, 0x7E, 9, 0xD9, 0xBC, 0x19, 0x8F, 0x1D, 0xD4, 0x3D, 0x37, 0xE1, 0x2F,
        0x15, 0x8D, 0x64, 6, 4, 0xFD, 0x29, 0xCF, 0xFA, 0x14, 0x2E, 0x25, 0x1F, 0xC9, 0x18, 0xE3, 0x6D, 0x81, 0x84,
        0x80, 0x3B, 0xB1, 0xE5, 0x4D, 0xF7, 0x51, 0x78, 0xA9, 0x13, 0xAD, 0xE9, 0x80, 0xC1, 0xB, 0x25, 0x93, 0xFC,
        0x4D, 0x89, 0x23, 0xC2, 0x7C, 0xB, 0x59, 0x15, 0xF6, 1, 0x50, 0x55, 0xBF, 0x81, 0x75, 0xC3, 0x10, 0x31, 0xB5,
        0xD3, 0x69, 0x84, 0x89, 0xBA, 0xD6, 0x89, 0xBD, 0x70, 0x19, 0x8E, 0x58, 0xA8, 0x3D, 0x9B, 0x5D, 0xF0, 0x49,
        0xE8, 0xAD, 0x9D, 0x7A, 0xD, 0x7E, 0x24, 0xDA, 0xFC, 0xD, 0x14, 0xC5, 0x23, 0x91, 0x11, 0xF5, 0xC0, 0x4B, 0xCD,
        0x44, 0x1C, 0xC5, 0x21, 0xDF, 0x61, 0x54, 0xED, 0xA2, 0x81, 0xB7, 0xE5, 0x74, 0x94, 0xB0, 0x47, 0xEE, 0xF1,
        0xA5, 0xBB, 0x21, 0xC8
};


struct usb_guncon3 {
	char name[128];
	char phys[65];
	struct usb_device *usbdev;
	struct input_dev *dev;
	struct urb *irq_in;
	struct urb *irq_out;
	int open;
    int ipipe, opipe;
    unsigned char key[8];

	unsigned char *idata;
	unsigned char *odata;
	dma_addr_t idata_dma;
	dma_addr_t odata_dma;
    struct mutex idata_mutex;
};

static const signed short guncon3_buttons[] = {
	BTN_TRIGGER,
	BTN_0, BTN_1,                   /* Buttons A1/A2 */
	BTN_2, BTN_3,                   /* Buttons B1/B2 */
	BTN_4, BTN_5,                   /* Buttons C1/C2 */
	BTN_6, BTN_7,                   /* Joystick buttons */
	-1
};

static const signed short guncon3_sticks[] = {
	ABS_X, ABS_Y,					/* Aiming */
    ABS_RX, ABS_RY,			/* Joystick A */
	ABS_HAT0X, ABS_HAT0Y,			/* Joystick B */
	-1
};

static struct usb_device_id usb_guncon3_id_table [] = {
	{match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,	idVendor: 0x0b9a, idProduct: 0x0800},
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_guncon3_id_table);

static int guncon3_decode(unsigned char *data, const unsigned char *key) {
    int x, y, key_index;
    unsigned char bkey, keyr, a_sum, b_sum, key_offset, byte;
    b_sum = ((data[13] ^ data[12]) + data[11] + data[10] - data[9] - data[8]) ^ data[7];
    a_sum = (((data[6] ^ b_sum) - data[5] - data[4]) ^ data[3]) + data[2] + data[1] - data[0];
    if (a_sum != key[7]) {
        if (debug)
            printk(KERN_ERR "checksum mismatch: %02x %02x\n", a_sum, key[7]);
        return -1;
    }

    key_offset = (((((key[1] ^ key[2]) - key[3] - key[4]) ^ key[5]) + key[6] - key[7]) ^ data[14]) + (unsigned char)0x26;

    key_index = 4;

    //byte E is part of the key offset
    // byte D is ignored, possibly a padding byte - make the checksum workout
    for (x = 12; x >= 0; x--) {
        byte = data[x];
        for (y = 4; y > 1; y--) { // loop 3 times
            key_offset--;

            bkey = KEY_TABLE[key_offset + 0x41];
            keyr = key[key_index];
            if (--key_index == 0)
                key_index = 7;

            if ((bkey & 3) == 0)
                byte =(byte - bkey) - keyr;
            else if ((bkey & 3) == 1)
                byte = ((byte + bkey) + keyr);
            else
                byte = ((byte ^ bkey) ^ keyr);
        }
        data[x] = byte;
    }
    return 0;
}

static void usb_guncon3_irq(struct urb *urb)
{
	struct usb_guncon3 *guncon3 = urb->context;
	unsigned char *data = guncon3->odata;
	struct input_dev *dev = guncon3->dev;
	int status, i;
	switch (urb->status) {
	case 0:			/* success */
		break;
	case -ECONNRESET:	/* unlink */
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	/* -EPIPE:  should clear the halt */
	default:		/* error */
		goto resubmit;
	}

    status = guncon3_decode(data, guncon3->idata);
    if (status != 0) {
        // TODO: submit new key
        printk(KERN_ERR "guncon3: checksum error\n");
        goto resubmit;
    }

	if (debug) {
		printk(KERN_INFO "guncon3_debug: data :");
		for(i = 0; i < 13; i++) {
			printk("0x%02x ", data[i]);
		}
		printk("\n");
	}
	/* aim */
	input_report_abs(dev, ABS_X, ((short)data[4] << 8) | (short)data[3]);
	input_report_abs(dev, ABS_Y, ((short)data[6] << 8) | (short)data[5]);
    // Z axis (data[8] << 8) + data[7]

	/* joystick a/b */
	input_report_abs(dev, ABS_RX, data[11]);
	input_report_abs(dev, ABS_RY, data[12]);
	input_report_abs(dev, ABS_HAT0X, data[9]);
	input_report_abs(dev, ABS_HAT0Y, data[10]);

    /* buttons */
    input_report_key(dev, BTN_TRIGGER, (data[1] & 0x20));
    input_report_key(dev, BTN_0, (data[0] & 0x04));    // A
    input_report_key(dev, BTN_1, (data[0] & 0x02));
    input_report_key(dev, BTN_2, (data[1] & 0x04));    // B
    input_report_key(dev, BTN_3, (data[1] & 0x02));
    input_report_key(dev, BTN_4, (data[1] & 0x80));    // C
    input_report_key(dev, BTN_5, (data[0] & 0x08));
    input_report_key(dev, BTN_6, (data[2] & 0x80));    // Joystick buttons
    input_report_key(dev, BTN_7, (data[2] & 0x40));

    input_sync(dev);

resubmit:
	status = usb_submit_urb(urb, GFP_ATOMIC);
	if (status)
		printk(KERN_ERR "can't resubmit intr, %s-%s/input0, status %d",
				guncon3->usbdev->bus->bus_name,
                guncon3->usbdev->devpath, status);

}

static void guncon3_irq_in(struct urb *urb) {
    switch (urb->status) {
        case 0:			/* success */
            if (debug)
                printk(KERN_INFO "guncon3: sent key\n");
            return;
        case -ECONNRESET:	/* unlink */
        case -ENOENT:
        case -ESHUTDOWN:
            return;
            /* -EPIPE:  should clear the halt */
        default:		/* error */
            break;
    }

    printk(KERN_ERR "guncon3: could not send key packet\n");
}

static int usb_guncon3_open(struct input_dev *dev)
{
	struct usb_guncon3 *guncon3 = input_get_drvdata(dev);
	int status;

    if (!guncon3)
        return -EIO;

	if (guncon3->open++)
		return 0;

	guncon3->irq_out->dev = guncon3->usbdev;
	if ((status = usb_submit_urb(guncon3->irq_out, GFP_KERNEL))) {
        if (debug)
		    printk(KERN_ERR "guncon3: %s open input urb failed: %d\n", guncon3->phys, status);
		guncon3->open--;
		return -EIO;
	}

    if (debug)
        printk(KERN_INFO "guncon3: open successful\n");

	return 0;
}

static void usb_guncon3_close(struct input_dev *dev)
{
	struct usb_guncon3 *guncon3 = input_get_drvdata(dev);

	if (!--guncon3->open) {
        usb_unlink_urb(guncon3->irq_out);
        usb_unlink_urb(guncon3->irq_in);
    }
}

static int guncon3_init_output(struct usb_interface *intf, struct usb_guncon3 *guncon3)
{
    struct usb_endpoint_descriptor *ep_irq_out;
    int error, maxp;

    guncon3->odata = usb_alloc_coherent(guncon3->usbdev, 15, GFP_ATOMIC, &guncon3->odata_dma);
    if (!guncon3->odata) {
        error = -ENOMEM;
        goto fail1;
    }

    guncon3->irq_out = usb_alloc_urb(0, GFP_KERNEL);
    if (!guncon3->irq_out) {
        error = -ENODEV;
        goto fail2;
    }

    ep_irq_out = &intf->cur_altsetting->endpoint[1].desc;
    maxp = usb_maxpacket(guncon3->usbdev, guncon3->opipe, usb_pipeout(guncon3->opipe));

    // setup the output irq
    usb_fill_int_urb(guncon3->irq_out, guncon3->usbdev, guncon3->opipe, guncon3->odata,
                     (maxp > 15 ? 15 : maxp),
                     usb_guncon3_irq, guncon3, ep_irq_out->bInterval);
    guncon3->irq_out->transfer_dma = guncon3->odata_dma;
    guncon3->irq_out->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    return 0;

fail2:
    usb_free_coherent(guncon3->usbdev, 8, guncon3->idata, guncon3->idata_dma);
fail1:
    printk(KERN_ERR "guncon3: error while setting up output irq\n");
    return error;
}

static int guncon3_init_input(struct usb_interface *intf, struct usb_guncon3 *guncon3)
{
    struct usb_endpoint_descriptor *ep_irq_in;
    int error, maxp;

    guncon3->idata = usb_alloc_coherent(guncon3->usbdev, 8,
                                        GFP_KERNEL, &guncon3->idata_dma);
    if (!guncon3->idata) {
        error = -ENOMEM;
        goto fail1;
    }

    mutex_init(&guncon3->idata_mutex);

    guncon3->irq_in = usb_alloc_urb(0, GFP_KERNEL);
    if (!guncon3->irq_in) {
        error = -ENOMEM;
        goto fail2;
    }

    ep_irq_in = &intf->cur_altsetting->endpoint[0].desc;
    maxp = usb_maxpacket(guncon3->usbdev, guncon3->ipipe, usb_pipeout(guncon3->ipipe));

    usb_fill_int_urb(guncon3->irq_in, guncon3->usbdev,
                     usb_sndintpipe(guncon3->usbdev, ep_irq_in->bEndpointAddress),
                     guncon3->idata, (maxp > 8 ? 8 : maxp),
                     guncon3_irq_in, guncon3, ep_irq_in->bInterval);
    guncon3->irq_in->transfer_dma = guncon3->idata_dma;
    guncon3->irq_in->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    return 0;

    fail2:
        usb_free_coherent(guncon3->usbdev, 8, guncon3->idata, guncon3->idata_dma);
    fail1:
        printk(KERN_ERR "guncon3: error while setting up input irq\n");
        return error;
}

static void guncon3_deinit_input(struct usb_guncon3 *guncon3)
{
    usb_free_coherent(guncon3->usbdev, 8, guncon3->idata, guncon3->idata_dma);
    usb_unlink_urb(guncon3->irq_in);
    usb_free_urb(guncon3->irq_in);
}

static void guncon3_deinit_output(struct usb_guncon3 *guncon3)
{
    usb_free_coherent(guncon3->usbdev, 8, guncon3->odata, guncon3->odata_dma);
    usb_unlink_urb(guncon3->irq_out);
    usb_free_urb(guncon3->irq_out);
}

static int guncon3_send_key(struct usb_guncon3 *guncon3)
{
    int retval;

    mutex_lock(&guncon3->idata_mutex);

    // TODO: generate key

    guncon3->key[0] = 0x01;
    guncon3->key[1] = 0x12;
    guncon3->key[2] = 0x6F;
    guncon3->key[3] = 0x32;
    guncon3->key[4] = 0x24;
    guncon3->key[5] = 0x60;
    guncon3->key[6] = 0x17;
    guncon3->key[7] = 0x21;
    memcpy(guncon3->idata, guncon3->key, 8);
    guncon3->irq_in->transfer_buffer_length = 8;

    if (debug)
        printk(KERN_INFO "guncon3: sending init packet to guncon...\n");

    retval = usb_submit_urb(guncon3->irq_in, GFP_KERNEL);

    mutex_unlock(&guncon3->idata_mutex);

    return retval;
}

static int usb_guncon3_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *usbdev = interface_to_usbdev(intf);
	struct usb_guncon3 *guncon3;
	struct input_dev *input_dev;
	struct usb_endpoint_descriptor *endpoint_in, *endpoint_out;
	struct usb_host_interface *interface;
	int i, error;
	char path[64];

	interface = intf->cur_altsetting;

	// has 2 endpoints, an input and an output
	if (interface->desc.bNumEndpoints != 2)
		return -ENODEV;

	// check the the endpoints are as expected
    endpoint_in = &interface->endpoint[0].desc;
    endpoint_out = &interface->endpoint[1].desc;

    if ((endpoint_in->bEndpointAddress & 0x80) || (endpoint_in->bmAttributes & 3) != 3 || \
       (!(endpoint_out->bEndpointAddress & 0x80) || (endpoint_out->bmAttributes & 3) != 3))
        return -ENODEV;

    // allocate memory fo the guncon data
    guncon3 = kzalloc(sizeof(struct usb_guncon3), GFP_KERNEL);
    if (!guncon3)
        return -ENOMEM;

	input_dev = input_allocate_device();
	if (!input_dev) {
		error = -ENOMEM;
        goto err_free_guncon;
	}
    guncon3->usbdev = usbdev;
    guncon3->dev = input_dev;

    guncon3->ipipe = usb_sndintpipe(usbdev, endpoint_in->bEndpointAddress);
    guncon3->opipe = usb_rcvintpipe(usbdev, endpoint_out->bEndpointAddress);

    error = guncon3_init_output(intf, guncon3);
    if (error)
        goto err_free_device;

    error = guncon3_init_input(intf, guncon3);
    if (error)
        goto err_free_output;

    // reports keys and absolute position
    __set_bit(EV_KEY, input_dev->evbit);
    __set_bit(EV_ABS, input_dev->evbit);

    // the buttons are keys
	for (i = 0; guncon3_buttons[i] >= 0; ++i)
        __set_bit(guncon3_buttons[i], input_dev->keybit);

    // the sticks are abs
	for (i = 0; guncon3_sticks[i] >= 0; ++i) {
		signed short t = guncon3_sticks[i];
        __set_bit(t, input_dev->absbit);
		switch (t) {
			case ABS_X: input_set_abs_params(input_dev, t, -32768, 32767, 16, 128); break;
			case ABS_Y: input_set_abs_params(input_dev, t, 32767, -32768, 16, 128); break;
			case ABS_RX:
            case ABS_RY:
            case ABS_HAT0X:
            case ABS_HAT0Y: input_set_abs_params(input_dev, t, 0, 255, 4, 8); break;
		}
	}

	usb_make_path(usbdev, path, 64);
	sprintf(guncon3->phys, "%s/input0", path);

	input_dev->name = guncon3->name;
	input_dev->phys = guncon3->phys;
	usb_to_input_id(usbdev, &input_dev->id);
	input_set_drvdata(input_dev, guncon3);
	input_dev->open = usb_guncon3_open;
	input_dev->close = usb_guncon3_close;

    sprintf(guncon3->name, "GunCon3");

    guncon3_send_key(guncon3);

	input_register_device(guncon3->dev);

	if (debug)
		printk(KERN_INFO "guncon3: input: %s on %s\n", guncon3->name, path);

	usb_set_intfdata(intf, guncon3);

	return 0;
    err_free_output:
        guncon3_deinit_output(guncon3);
    err_free_device:
        input_free_device(input_dev);
    err_free_guncon:
        kfree(guncon3);
        printk(KERN_ERR "guncon3: error while setting up device\n");
        return error;
}

static void usb_guncon3_disconnect(struct usb_interface *intf)
{
	struct usb_guncon3 *guncon3 = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);
	if (guncon3) {
		guncon3_deinit_input(guncon3);
		guncon3_deinit_output(guncon3);
        input_set_drvdata(guncon3->dev, NULL);
		kfree(guncon3);
	}
}

static struct usb_driver usb_guncon3_driver = {
	.name		= "guncon3",
	.probe		= usb_guncon3_probe,
	.disconnect	= usb_guncon3_disconnect,
	.id_table	= usb_guncon3_id_table,
};

static int __init usb_guncon3_init(void)
{
	int retval = usb_register(&usb_guncon3_driver);
	if (retval == 0)
		printk(KERN_INFO "guncon3: " DRIVER_DESC " " DRIVER_VERSION " initialized\n");
	return retval;
}

static void __exit usb_guncon3_exit(void)
{
	usb_deregister(&usb_guncon3_driver);
}

module_init(usb_guncon3_init);
module_exit(usb_guncon3_exit);
