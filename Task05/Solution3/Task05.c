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

static int __init hello_init(void)
{
	pr_info("HelloModule: i am loading\n");
	pr_debug("HelloModule: Hello World\n");

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, i am unloading\n");
}

module_init(hello_init);
module_exit(hello_exit);
