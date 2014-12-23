#!/bin/sh

if [ "$ACTION" = "add" ]; then
	insmod /home/parham/bin/Task05.ko
fi
if [ "$ACTION" = "remove" ]; then
	rmmod Task05
fi
