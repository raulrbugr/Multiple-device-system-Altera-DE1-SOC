/*
 * lector.h: Implements the /dev/accel reader to get the accelerometer values.
 *
 * Note: It is necessary install the accelerometer module. 
 *	It is provided in /KernelAccel directory of this project.
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define BYTES 256  // max # of bytes to read from /dev/acceldev <<<<<<<<<<<<<<<<<<<<<<<<
#define PATH_FILE "/dev/accel"


/***************** Variables ***************/

volatile sig_atomic_t stop;

/***************** Functions prototipes ***************/

void catchSIGINT(int signum);
int runAccelerometer(int *val_X, int *val_Y, int *val_Z );