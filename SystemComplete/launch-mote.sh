#! /bin/bash

MOD_INS="$(lsmod | grep accel)"

if [ -z "$MOD_INS" ]
then
    echo "inserting module..."
    insmod ./KernelAccel/accel.ko
else
    echo "module already inserted"
fi


make clean

make

source /home/tinyosroot/tinyos-release-tinyos-2_1_2/tinyos.env

seriallisten /dev/ttyACM0 57600 | ./system_sample
