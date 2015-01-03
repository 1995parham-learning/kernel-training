#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani");
MODULE_DESCRIPTION("This module does nothing .....\n");

static struct usb_device_id hello_id_table[] = {
		{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
				USB_INTERFACE_SUBCLASS_BOOT,
				USB_INTERFACE_PROTOCOL_KEYBOARD)},
		{} /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, hello_id_table);

static int hello_probe(struct usb_interface *interface,
		    const struct usb_device_id *id)
{
	pr_info("HelloModule: USB keyboard probe function called\n");
	return 0;
}

static void hello_disconnect(struct usb_interface *interface)
{
	pr_info("HelloModule: USB keyboard disconnect function called\n");
}

static struct usb_driver hello_driver = {
	.name =		"hello_driver",
	.probe =	hello_probe,
	.disconnect =	hello_disconnect,
	.id_table =	hello_id_table
};

static int __init hello_init(void)
{
	int retval = 0;

	pr_info("HelloModule: i am loading\n");
	pr_info("HelloModule: Hello World\n");

	retval = usb_register(&hello_driver);
	if (retval) {
		pr_info("HelloModule: usb_register failed. Error number %d",
				retval);
		return retval;
	}

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, i am unloading\n");
	usb_deregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);
