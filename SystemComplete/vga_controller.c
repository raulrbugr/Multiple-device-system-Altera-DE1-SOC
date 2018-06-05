/*
 * vga_controller.c: Implements the vga functions.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include "vga_controller.h"

/*
 * Initialize the VGA memory pointers and resolution
 */
int init_VGA(int fd, void *LW_virtual){
	
	if ((fd = open_physical (fd)) == -1)
		return (-1);

	
	//Maping
	if ((FPGAMEM_virtual = map_physical (fd, FPGA_ONCHIP_BASE , FPGA_ONCHIP_SPAN)) == NULL)
		return (-1);
	
	FPGAMEM_ptr = (unsigned short int *)FPGAMEM_virtual;
	
	if ((SDRAM_virtual = map_physical (fd, SDRAM_BASE , SDRAM_SPAN)) == NULL)
		return (-1);
	if ((FPGA_CHAR_virtual = map_physical (fd, FPGA_CHAR_BASE , FPGA_CHAR_SPAN)) == NULL)
		return (-1);
	
	
	close_physical(fd);
	//if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE , LW_BRIDGE_SPAN)) == NULL)
		//return (-1);
	
	pixel_ctrl_ptr = (unsigned int *)(LW_virtual + PIXEL_BUF_CTRL_BASE); 
	char_ctrl_ptr = (unsigned int *)(FPGA_CHAR_virtual);  	
	
	
	// Set start physical direction in the memory for buffer
	*pixel_ctrl_ptr  = FPGA_ONCHIP_BASE;
	
	// Set start physical direction in the memory for backbuffer
	*(pixel_ctrl_ptr + 1) = SDRAM_BASE;
	
	//Resolution
	//0000000011110000 240 -- 101000000 320 -- 111100000 480 -- 1010000000 640;
	// 31 downto 16 Y  15 downto 0 X
	//*(pixel_ctrl_ptr + 2) = 0b00000001111000000000001010000000; //480Y 640X
	*(pixel_ctrl_ptr + 2) = 0b00000000111100000000000101000000; //240Y 320X
	
	
	return 0;
}

/*
 * Unmap the VGA pointers from the memory
 */
void finishVGA(void){
	
	unmap_physical(FPGAMEM_virtual,FPGA_ONCHIP_SPAN);
	unmap_physical(SDRAM_virtual,SDRAM_SPAN);
	unmap_physical(FPGA_CHAR_virtual,FPGA_CHAR_SPAN);
	
	

}



/*
 * Function to wait the synchronization
 */
void wait_for_vsync(void){
	
	*pixel_ctrl_ptr  = 1;
	
	while((*(pixel_ctrl_ptr+3) & 0x1) ==1);

}


/*
 * Function to use the front and back buffers
 */
 
 void draw_pixel_2Buffers(int x, int y, short int line_color){
	 
	 
	 if(*pixel_ctrl_ptr==FPGA_ONCHIP_BASE){
		 //posicion escritura  SDRAM_BASE
		*(short int *)(SDRAM_virtual + (y << 10) + (x << 1)) = line_color;
		//printf("SDRAM");
	 } else{
		*(short int *)(FPGAMEM_virtual + (y << 10) + (x << 1)) = line_color;
		//printf("FPGAMEM");
	 }
		 
	 
 }

 
 /*
  * Function to write the header names on the screen
  */
void write_headers(void){
	write_data("Pablo San Jose Burgos", NAMES_X, NAME1_Y);
	write_data("Raul Ruiz Bueno", NAMES_X,NAME2_Y);
	write_data("Asignatura: \"Buses\"", NAMES_X, SUBJCT_Y);
	write_data("Temperatura: ", DATA_X, TEMP_LINE_Y);
	write_data("Humedad:     ", DATA_X, HUM_LINE_Y);
}

/*
 * Function to use the front buffer
 */
 
void draw_pixel(int x, int y, short int line_color)
{
	*(short int *)(FPGAMEM_virtual + (y << 10) + (x << 1)) = line_color;
}


/* 
 * Function that writes a single character to the character buffer at x,y
 */
void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (char_ctrl_ptr + (y<<7) + x);
  *character_buffer = c;
}

/*
 * Function that writes a string on the screen
 */ 
void write_data(char *data, int start_x, int start_y){
	char *ptr_char2print;
	int x;
	
	ptr_char2print = data;
	x = start_x;
	while (*ptr_char2print){
		write_char(x, start_y, *ptr_char2print);
		x++;
		ptr_char2print++;
	}
}

/*
 * Function to clear char data in the screen
 */
void clear_data(void){
	int i, j;
	for(i=0;i< 80; i++){
		for(j=0;j< 60; j++){
			write_char(i,j,0); // FFFF
		}
	}
}


/*
 * Function to draw a line 
 */
void  draw_line(int x0, int x1, int y0, int y1, short int line_color ){

	bool is_steep = abs(y1 - y0) > abs(x1 - x0);
	
	
	int swap, y_step, x;
	
	
	
	if (is_steep){ 
		//swap(x0, y0)
		//swap(x1, y1)
		
		swap=x0;
		x0=y0;
		y0=swap;
		
		swap=x1;
		x1=y1;
		y1=swap;
	 
	}
	if (x0 > x1){
		//swap(x0, x1)
		//swap(y0, y1)
		
		swap=x0;
		x0=x1;
		x1=swap;
		
		swap=y1;
		y1=y0;
		y0=swap;
		 
		 
	}
	
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = -(deltax / 2);
	int y = y0;
 
	if (y0 < y1)  
		y_step = 1; 
	else 
		y_step = -1;

	for(x=x0; x<x1; x++){
		
		if (is_steep) 
			draw_pixel_2Buffers(y,x,line_color); 
		else 
			draw_pixel_2Buffers(x,y,line_color);
		
		error = error + deltay;
		
		if (error>= 0){ 
			y = y + y_step;
			error = error - deltax;
		}
	}
	
	//Vsync
	//wait_for_vsync();
}

/*
 * Function to clear the screen
 */
 
 void clear_screen(void){
	int i,j;
	
	for(i=0;i< 320; i++){
		for(j=0;j< 240; j++){
			draw_pixel_2Buffers(i,j,0); // FFFF
		}
	}
 }
 
