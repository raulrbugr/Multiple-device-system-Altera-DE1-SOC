#include "adc.h"

/********************************************************************************
 * This program demonstrates use of the ADC port in the DE1-SoC and DE0-Nano-SoC
 * Computers.
 *
 * It performs the following:
 * 1. Performs a conversion operation on all eight channels.
 * 2. Displays the converted values on the terminal window.
********************************************************************************/

int initADC(int fd, void *LW_virtual){
	
		

	
	//Maping
	//if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE , LW_BRIDGE_SPAN)) == NULL)
		//return (-1);
	
	
	ADC_ptr = (unsigned int *)(LW_virtual+ADC_BASE);
	
	return 0;
	
}


void closeADC(void){
	
	//unmap_physical(LW_virtual,LW_BRIDGE_SPAN);
	
	
}


int runADC(void)
{
	volatile int delay_count;	// volatile so that the C compiler doesn't remove the loop
	int port, value,i;
	*(ADC_ptr + 1) = 1;	// Sets the ADC up to automatically perform conversions.
	for(i=0; i < 500 ; i++)
	{
		printf("\033[H"); // sets the cursor to the top-left of the terminal window
		for (port = 0; port < 8; ++port)
		{
			value = *(ADC_ptr + port);
			printf ("ADC port %d: 0x%x\n", port, value);
		}
		for (delay_count = 1500000; delay_count != 0; --delay_count); // delay loop
	}
	
	return 0;
}
