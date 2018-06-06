# Multiple device system Altera DE1-SOC


Project developed for ALTERA Cyclone V DE1-SoC

Authors:
Pablo San José Burgos & Raúl Ruiz Bueno

## Implemented modules

### Accelerometer and accelerometer reader

Driver for the ADXL345 accelerometer using the I2C bus. It writes the values in /dev/accel

### VGA

VGA driver using front and back buffer to obtain better performance.

### ADC

Using the SPI bus the application take the pins analog values and convert it to digital values.

### Mote

Using two motes ( one sender and other receiver) we can take humidity and temperature values.

## Test application

![](https://www.dropbox.com/s/ed13qefigsc7pq4/testapp.JPG?dl=0)

https://www.youtube.com/watch?v=tzVfwGzz6Hk

### Accelerometer and accelerometer reader

In the lower right quadrant of Figure a rectangle is drawn, using the
accelerometer and the reader transform the values ​​of the accelerometer to positions of the
pixels giving a movement effect.

### VGA

Several examples are drawn on the screen to check the correct execution of the application
and its modules.

### ADC

In the upper right quadrant of Figure a graph is drawn representing the
change of values ​​in channel 0 of the ADC. For this project, we use a
potentiometer to vary the values.

### Mote

Temperature and humidity values ​​are displayed in the lower left quadrant
captured from the mote.
