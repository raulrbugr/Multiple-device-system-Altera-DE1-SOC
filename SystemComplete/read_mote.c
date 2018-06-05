/*
 * read_mote.c: Implements functions to read char packages from the mote.
 *
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include "read_mote.h"

/*
 * Function to retreive a packet from the pipe
 */
int get_packet(char *message){

    char buf[BUF_SIZE];
    int i = 0;
    int flag_header_dtc = 0;
    int state = 0;

    fscanf(stdin, "%s", buf);

    
    for (i = 0; i < BUF_SIZE; i++){
        buf[i] = 0x0;
    }

    while(flag_header_dtc == 0){

        switch (state) {
            case 0:
                fscanf(stdin, "%s", buf);

                if(strcmp(buf, HEADER_SEQ1) == 0){
                    state = 1;
                }
              break;

            case 1:
                fscanf(stdin, "%s", buf);

                if(strcmp(buf, HEADER_SEQ2) == 0){
                state = 2;
                } else{
                    state = 0;
                }
              break;

            case 2:
                fscanf(stdin, "%s", buf);

                if(strcmp(buf, HEADER_SEQ2) == 0){
                state = 3;
                } else{
                    state = 0;
                }
              break;

            case 3:
                fscanf(stdin, "%s", buf);
				
                if(strcmp(buf, HEADER_SEQ1) == 0){
                state = 4;
                } else{
                    state = 0;
                }
              break;

            case 4:
                flag_header_dtc = 1;

                for (i = 0; i < (FRAME_SIZE/2)-4; i++){
                    fscanf(stdin, "%s", buf);
                    strcat(message, buf);
                }
				printf("%s\n", message);
				state = 0;

            break;


            default:
              state = 0;
        }
    }
    
    return 0;

}


/*
 * Function to extract the NodeID, temperature and hummidity from the message passed
 */
int extract_data(char *message, char *id_array, char *temp_array, char *hum_array){

    int pos = HEADER_SIZE;
    int i;

    // Extract ID
    i=0;
    while (pos < END_NODE_ID){
        id_array[i] = message[pos];
        i++;
        pos++;
    }

    id_array[FIELD_SIZE] = '\0';

    // Extract temperature
    pos += (3*FIELD_SIZE);
    i=0;
    while(pos < END_TEMP){
        temp_array[i] = message[pos];
        i++;
        pos++;
    }

    temp_array[FIELD_SIZE] = '\0';

    // Extract hummidity
    i=0;
    while(pos < END_HUM){
        hum_array[i] = message[pos];
        i++;
        pos++;
    }

    hum_array[FIELD_SIZE] = '\0';

    return 0;
}
