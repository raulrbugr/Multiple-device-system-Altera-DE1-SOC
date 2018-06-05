/*
 * adcSPI.h: Implements the ADC driver using the SPI bus.
 *
 * Note: It is necessary charge the .sof file in the FPGA. 
 *	It is provided in /Config directory of this project.
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */
 
#include "altera_avalon_spi_regs.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)

#include "alt_types.h"  // alt_u32
#include "address_map_arm.h"
#include "maping.h"


/****************** Variables ******************/

volatile unsigned int * SPI_0_BASE;



/****************** Functions ******************/

void initADC( void *LW_virtual);
int runADCspi(alt_u16 *v_adc_result);