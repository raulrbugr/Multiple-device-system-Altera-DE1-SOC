/*
 * adcSPI.c: Implements the ADC driver using the SPI bus.
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


#include "adcSPI.h"
//#include "alt_types.h"  // alt_u32


/*
 * Function to initialize SPI pointers
 */
void initADC( void *LW_virtual){
	
	
	SPI_0_BASE = (unsigned int *)(LW_virtual + ADC_SPI_BASE);  
	
	
}

/*
 * Function to catch ADC values
 */

int runADCspi(alt_u16 *v_adc_result){
	
     alt_u16 adc_result;
     //alt_u16 v_adc_result[100];
     alt_u16 status=0x00;
	int i;
		

		
     //printf("channel 0-7 voltage value:\n");

       //initial the spi controller
		IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SPI_0_BASE, 0x1); //select device 0 (Mask 0x0001)
		IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_0_BASE, 0x00);  //reset the control reg
		IOWR_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE, 0x00);   // Clear the Status Reg
		//initial the adc after power supply
	 
	
	   //tx two cycle dummy bits
        while(!(IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_TRDY_MSK));
		IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, 0xffff); //the first cycle dummy bits

	    while(!(IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_TRDY_MSK));
	    IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, 0xffff); //the second cycle dummy bits
	
	    // SEQ Bit = 1, SHADOW Bit = 1
	    while(!(IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_TRDY_MSK)); //write control and sequence register
	    IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, 0xdf90); //datasheet figure 13,config the control reg, channel 0-7
	       // 0xdf90  = 1101 1111 1001 0000
	       //      WRITE SEQ -- ADD2 ADD1 ADD0 PM1 PM0 SHADOW -- RANGE CODING -- -- -- --
	       //        1    1   0   1    1    1   1   1    1     0    0     1    0  0  0  0


	    //read converting result from channel 0 - channel 7
	
	 for(i=0;i<8;i++){
		 // if(i==0)
		 //   usleep(1); //matthew add

	     while(!(IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_TRDY_MSK)); //write control and sequence register
	     IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE, 0x5f90); //read the convert result channel 0
	       // 0x5f90  = 0101 1111 1001 0000
	       //      WRITE SEQ -- ADD2 ADD1 ADD0 PM1 PM0 SHADOW -- RANGE CODING -- -- -- --
	       //        0    1   0   1    1    1   1   1    1     0    0     1    0  0  0  0

	       // wait tx done
	     status=0x00;
	     while (!(status & ALTERA_AVALON_SPI_STATUS_TMT_MSK)){
	          status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);
	     }

	    while(!(IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE) & ALTERA_AVALON_SPI_STATUS_RRDY_MSK));
	    adc_result = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_0_BASE);

	    v_adc_result[i] = adc_result;
	    // printf("CH%d = %fv %04X\n",(adc_result>>12)&0x7,2*2.5*(adc_result&0x0fff)/4096, adc_result);

    }//end for
	//for(i=0;i<8;i++) printf("CH%d = %fv %04X\n",(v_adc_result[i]>>12)&0x7,2*2.5*(v_adc_result[i]&0x0fff)/4096, v_adc_result[i]);

	
	return 0;
 }