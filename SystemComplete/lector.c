/*
 * lector.c: Implements the /dev/accel reader to get the accelerometer values.
 *
 * Note: It is necessary install the accelerometer module. 
 *	It is provided in /KernelAccel directory of this project.
 *
 * Authors:
 *
 * Pablo San José Burgos and Raúl Ruiz Bueno.
 *
 * 22/05/2018
 *
 */

#include "lector.h"


void catchSIGINT(int signum){
    stop = 1;
}


/* 
 * Function to open /dev/accel and read the values from the acelerometer kernel module
 */
int runAccelerometer(int *val_X, int *val_Y, int *val_Z ){

    int acceldev_FD;
    char X_AXIS[] = "XXX";
    char Y_AXIS[] = "YYY";
    char Z_AXIS[] = "ZZZ";

    int i = 0;

    // file descriptor
    char acceldev_buffer[BYTES]; // buffer for acceldev character data
    int ret_val, chars_read;

    // catch SIGINT from ^C, instead of having it abruptly close this program
    signal(SIGINT, catchSIGINT);
    // Open the character device driver for read/write
    if ((acceldev_FD = open(PATH_FILE, O_RDWR)) == -1) {
        printf("Error opening /dev/accel: %s\n", strerror(errno));
        return -1;
    }


    //for(j=0; j<10; j++) {
        chars_read = 0;
        while ((ret_val = read (acceldev_FD, acceldev_buffer, BYTES)) != 0){
            chars_read += ret_val; // read the driver until EOF
        }
        
        acceldev_buffer[chars_read] = '\0'; // NULL terminate
        //printf ("%s\n", acceldev_buffer);
        for (i = 0; i<4;i++){
            X_AXIS[i] = acceldev_buffer[i];
        }
        
        for (i = 4; i<8;i++){
            Y_AXIS[i-4] = acceldev_buffer[i];
        }
        
        for (i = 8; i<12;i++){
            Z_AXIS[i-8] = acceldev_buffer[i];
        }

        *val_X = atoi(X_AXIS);
        *val_Y = atoi(Y_AXIS);
        *val_Z = atoi(Z_AXIS);

       /* printf ("X_AXIS: %s\n", X_AXIS);
        printf ("Y_AXIS: %s\n", Y_AXIS);
        printf ("Z_AXIS: %s\n", Z_AXIS);*/

        /*printf ("val_X: %d\n", *val_X);
        printf ("val_Y: %d\n", *val_Y);
        printf ("val_Z: %d\n", *val_Z);*/
        
        //sleep (1);
    //}
    close (acceldev_FD);
    return 0;
}
