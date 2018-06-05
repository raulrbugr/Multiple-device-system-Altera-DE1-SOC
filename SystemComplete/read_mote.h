/*
 * read_mote.h: Implements functions to read char packages from the mote.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUF_SIZE    3
#define FRAME_SIZE  52
#define HEADER_SIZE 16
#define END_NODE_ID 20
#define END_TEMP    36
#define END_HUM     40
#define FIELD_SIZE  4
#define FIELD_STR   FIELD_SIZE + 1
#define HEADER      "00ffff00"

#define HEADER_SEQ1	"00"
#define HEADER_SEQ2 "ff"

/***************** Variables ***************/



int get_packet(char *message);

/***************** Functions prototipes ***************/

int extract_data(char *message, char *id, char *tempeature, char *humidity);
int extract_frame_data_int(char *message, int *id, int *temp, int *hum);