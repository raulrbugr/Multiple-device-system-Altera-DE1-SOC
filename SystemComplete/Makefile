#
TARGET = system_sample

#
CROSS_COMPILE = arm-linux-gnueabihf-
#CFLAGS = -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
CFLAGS = -g -Wall  
LDFLAGS =  -g -Wall 
CC = $(CROSS_COMPILE)gcc
ARCH= arm

build: $(TARGET)

system_sample: main.o vga_controller.o maping.o adcSPI.o lector.o read_mote.o
	$(CC) $(LDFLAGS)   $^ -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean 
clean:
	rm -f $(TARGET) *.a *.o *~


