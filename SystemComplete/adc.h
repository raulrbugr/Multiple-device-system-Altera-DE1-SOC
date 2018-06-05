#include <stdio.h>
#include "address_map_nios2.h"
#include "maping.h"



/*******************  Variables  ********************/

volatile unsigned int *ADC_ptr;
void * LW_virtual;
		
/******************* Functions  ********************/

int runADC(void);
int initADC(int fd, void *LW_virtual);
void closeADC(void);