/*
 * main.h: application main header.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "vga_controller.h"
#include "adcSPI.h"
#include "maping.h"
#include "lector.h"
#include "read_mote.h"
//#include "systemcontroller.h"



// structure for message queue
struct mesg_buffer {
    long msg_type;
    char msg_node_id[FIELD_STR];
	char msg_temp[FIELD_STR];
	char msg_hum[FIELD_STR];
};

/***************** Variables ***************/

typedef struct mesg_buffer msg_type;

/***************** Functions prototipes ***************/
