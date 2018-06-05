/*
 * main.c: application main file.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include "main.h"

int msgid;
int child_pid;

void sig_handler(int sgn){
	
	//Close, unmap....
	printf("signal processing...")
	kill(child_pid,SIGTERM); // Terminar el proceso hijo
	finishVGA();
	msgctl(msgid, IPC_RMID, NULL);
	unmap_physical(LW_virtual,LW_BRIDGE_SPAN);
	close_physical(fd);
}


int main(void){
	
	child_pid = fork();
	
	if (child_pid == 0){
		// Child, reader
		
		msg_type message_send;
		char packet[FRAME_SIZE+1];
		
		key_t key;
		int msgid;
	 
		// ftok to generate unique key
		key = ftok("progfile", 65);
	 
		// msgget creates a message queue, returns id
		msgid = msgget(key, 0666 | IPC_CREAT);
		message_send.msg_type = 1;
		
		while(1){
			// Get mote data
			strcpy(packet, HEADER);
			get_packet(packet);
			extract_data(packet, message_send.msg_node_id, message_send.msg_temp, message_send.msg_hum);
			
			if (strcmp(message_send.msg_node_id, "0015") == 0) msgsnd(msgid, &message_send, sizeof(msg_type), 0);
			
		}
		
	} else { 
		// Father, reader 
		int fd=-1;
		int xini=230,yini=170, x, y;
		
		int i,t;
		
		int val_X = -1;
		int val_Y = -1;
		int val_Z = -1;

		alt_u16 v_adc_result[100];
		
		int ch1[5];
		
		key_t key;

		msg_type message_rcv;
		
		signal(SIGINT, sig_handler);
		signal(SIGTERM, sig_handler);
		signal(SIGQUIT, sig_handler);
	 
		// ftok to generate unique key
		key = ftok("progfile", 65);
	 
		// msgget creates a message queue, returns id
		msgid = msgget(key, 0666 | IPC_CREAT);
		
		//Map the components
		if ((fd = open_physical (fd)) == -1)
			return (-1);

			
		if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE , LW_BRIDGE_SPAN)) == NULL)
			return (-1);
		
		//Init components
		initADC(LW_virtual)	;
		init_VGA(fd,LW_virtual);
		
			
		clear_screen();
		
		for( i=0; i < 5; i++){
			ch1[i]=0;
		}
		 
		//draw screen
		draw_line(0, 319, 120, 120, 0x00bfff); // this line is blue
		draw_line(160, 160, 0, 239, 0x00bfff); // this line is blue
		wait_for_vsync(); 

		clear_data();
		write_headers();
		
		for (t = 0; t < 300; t++){
			
			// msgrcv to receive message
			if(msgrcv(msgid, &message_rcv, sizeof(msg_type), 1, IPC_NOWAIT) != -1){
				
				write_data(message_rcv.msg_temp, DATA_X+DATA_TITLE_SIZE, TEMP_LINE_Y);
				write_data(message_rcv.msg_hum, DATA_X+DATA_TITLE_SIZE, HUM_LINE_Y);
				
			}
			
			//write_data(, 0, 0);

			
			clear_screen();
			
			//Draw the divisions
			
			draw_line(0, 319, 120, 120, 0x00bfff); 
			draw_line(160, 160, 0, 239, 0x00bfff); 
			
			//Obtain accelerometer values
			
			runAccelerometer(&val_X,&val_Y,&val_Z); 
			
			//Draw the accelerometer test square
			x=xini+val_X/50;
			y=yini+val_Y/50;
			
			draw_line(x, x+SIZE_X, y, y, 0x1DF0); 
			draw_line(x, x+SIZE_X, y+SIZE_Y, y+SIZE_Y, 0x1DF0);
			draw_line(x, x, y, y+SIZE_Y, 0x1DF0);
			draw_line(x+SIZE_X, x+SIZE_X, y, y+SIZE_Y+1, 0x1DF0);
			
			
			//Obtain ADC values
			runADCspi(v_adc_result);
			ch1[0]=2*2.5*(v_adc_result[0]&0x0fff)/4096;
			
			
			//Draw the ADC test graphic
			draw_line(319,280 , 60-ch1[0]*5, 60-ch1[1]*5, 0x1DF0);
			draw_line(280,240 , 60-ch1[1]*5, 60-ch1[2]*5, 0x1DF0);
			draw_line(240,200 , 60-ch1[2]*5, 60-ch1[3]*5, 0x1DF0);
			draw_line(200,160 , 60-ch1[3]*5, 60-ch1[4]*5, 0x1DF0);
			
			ch1[4]=ch1[3];
			ch1[3]=ch1[2];
			ch1[2]=ch1[1];
			ch1[1]=ch1[0];
			
			
			//Synchronize
			wait_for_vsync(); 
		
		}
		
		//runADCspi(v_adc_result);
		//for(i=0;i<8;i++) printf("CH%d = %fv %04X\n",(v_adc_result[i]>>12)&0x7,2*2.5*(v_adc_result[i]&0x0fff)/4096, v_adc_result[i]);
		
		
		//Close, unmap....
		kill(child_pid,SIGTERM); // Terminar el proceso hijo
		finishVGA();
		msgctl(msgid, IPC_RMID, NULL);
		unmap_physical(LW_virtual,LW_BRIDGE_SPAN);
		close_physical(fd);
	} // End father
	
	return 0;
	
}