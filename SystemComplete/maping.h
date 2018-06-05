/*
 * mapping.h: Implements the map and unmap funtions.
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
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>


/********** Variables ************************/



/********** Function prototipes **************/
int open_physical (int fd);
void close_physical (int fd);
void* map_physical(int fd, unsigned int base, unsigned int span);
int unmap_physical(void * virtual_base, unsigned int span);
