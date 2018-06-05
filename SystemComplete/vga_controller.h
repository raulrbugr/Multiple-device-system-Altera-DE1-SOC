/*
 * vga_controller.h: Implements the vga functions.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include "address_map_arm.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "maping.h"

/*
* 0xC8000000 address pixel buffer
*
* Using this scheme, the pixel at location 0,0 has the address 0xC8000000, the pixel 
* 1,0 has the address base + (00000000 000000001 0)2 = 0xC8000002
*
* The pixel 0,1 has the address base + (00000001 000000000 0)2 = 0xC8000400
*
* The pixel at location 319,239 has the address base +(11101111 100111111 0)2 = 0xC803BE7E
* 
*/


/*
* The register at address 0xFF203020 is called the Buffer register
* 0xFF203024 Backbuffer register
* 0xFF203028 Resolution register
* 0xFF20302C Status register
*
*/


/*
* To use the front and back buffer VGA_STATUS the bit 0 ---> 0b1
*
*/

/****************DEFINEs******************************/
#define VGA_BUFFER 		LW_BRIDGE_BASE + PIXEL_BUF_CTRL_BASE
#define VGA_BACKBUFFER 	LW_BRIDGE_BASE + PIXEL_BUF_CTRL_BASE + 0x4
#define VGA_RESOLUTION 	LW_BRIDGE_BASE + PIXEL_BUF_CTRL_BASE + 0x8
#define VGA_STATUS 		LW_BRIDGE_BASE + PIXEL_BUF_CTRL_BASE + 0xC

#define VGA_BASE_ADDRESS 	
#define LIMIT_X 319
#define LIMIT_Y 239
#define SIZE_X 	20
#define SIZE_Y	20
#define NAMES_X 1
#define NAME1_Y 5
#define NAME2_Y 7
#define SUBJCT_Y 9
#define DATA_X 5
#define TEMP_LINE_Y 40
#define HUM_LINE_Y 42
#define DATA_TITLE_SIZE 13


/***************Global variables************************/

volatile unsigned int * pixel_ctrl_ptr;
volatile unsigned int char_ctrl_ptr;

void *LW_virtual;

void *FPGAMEM_virtual;
volatile unsigned short int * FPGAMEM_ptr;

void *SDRAM_virtual;
void *FPGA_CHAR_virtual;

/**************Function prototipes***********************/
int init_VGA(int fd, void *LW_virtual);
void finishVGA(void);
void draw_line(int x0, int x1, int y0, int y1, short int line_color);
void draw_pixel_2Buffers(int x, int y, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void clear_screen(void);
void wait_for_vsync(void);
void write_char(int x, int y, char c);
void write_data(char *data, int start_x, int start_y);
void clear_data(void);
void write_headers(void);

