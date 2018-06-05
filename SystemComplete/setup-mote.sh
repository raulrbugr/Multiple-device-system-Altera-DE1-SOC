#!/bin/bash

modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm

source /home/tinyosroot/tinyos-release-tinyos-2_1_2/tinyos.env

DEVNAME="$(ls /dev/ttyACM*)"

echo "Mote location: $DEVNAME"

echo "testing..."

seriallisten $DEVNAME 57600
