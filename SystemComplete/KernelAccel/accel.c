#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>

/* Bit values in BW_RATE                                                */
/* Expresed as output data rate */
#define XL345_RATE_3200       0x0f
#define XL345_RATE_1600       0x0e
#define XL345_RATE_800        0x0d
#define XL345_RATE_400        0x0c
#define XL345_RATE_200        0x0b
#define XL345_RATE_100        0x0a
#define XL345_RATE_50         0x09
#define XL345_RATE_25         0x08
#define XL345_RATE_12_5       0x07
#define XL345_RATE_6_25       0x06
#define XL345_RATE_3_125      0x05
#define XL345_RATE_1_563      0x04
#define XL345_RATE__782       0x03
#define XL345_RATE__39        0x02
#define XL345_RATE__195       0x01
#define XL345_RATE__098       0x00

/* Bit values in DATA_FORMAT                                            */

/* Register values read in DATAX0 through DATAZ1 are dependant on the 
   value specified in data format.  Customer code will need to interpret
   the data as desired.                                                 */
#define XL345_RANGE_2G             0x00
#define XL345_RANGE_4G             0x01
#define XL345_RANGE_8G             0x02
#define XL345_RANGE_16G            0x03
#define XL345_DATA_JUST_RIGHT      0x00
#define XL345_DATA_JUST_LEFT       0x04
#define XL345_10BIT                0x00
#define XL345_FULL_RESOLUTION      0x08
#define XL345_INT_LOW              0x20
#define XL345_INT_HIGH             0x00
#define XL345_SPI3WIRE             0x40
#define XL345_SPI4WIRE             0x00
#define XL345_SELFTEST             0x80

/* Bit values in INT_ENABLE, INT_MAP, and INT_SOURCE are identical
   use these bit values to read or write any of these registers.        */
#define XL345_OVERRUN              0x01
#define XL345_WATERMARK            0x02
#define XL345_FREEFALL             0x04
#define XL345_INACTIVITY           0x08
#define XL345_ACTIVITY             0x10
#define XL345_DOUBLETAP            0x20


#define XL345_SINGLETAP            0x40
#define XL345_DATAREADY            0x80

/* Bit values in POWER_CTL                                              */
#define XL345_WAKEUP_8HZ           0x00
#define XL345_WAKEUP_4HZ           0x01
#define XL345_WAKEUP_2HZ           0x02
#define XL345_WAKEUP_1HZ           0x03
#define XL345_SLEEP                0x04
#define XL345_MEASURE              0x08
#define XL345_STANDBY              0x00
#define XL345_AUTO_SLEEP           0x10
#define XL345_ACT_INACT_SERIAL     0x20
#define XL345_ACT_INACT_CONCURRENT 0x00

// Register List
#define ADXL345_REG_DEVID       0x00
#define ADXL345_REG_POWER_CTL   0x2D
#define ADXL345_REG_DATA_FORMAT 0x31
#define ADXL345_REG_FIFO_CTL    0x38
#define ADXL345_REG_BW_RATE     0x2C
#define ADXL345_REG_INT_ENALBE  0x2E  // default value: 0x00
#define ADXL345_REG_INT_MAP     0x2F  // default value: 0x00
#define ADXL345_REG_INT_SOURCE  0x30  // default value: 0x02
#define ADXL345_REG_DATA_FORMAT 0x31  // defuault value: 0x00
#define ADXL345_REG_DATAX0      0x32  // read only
#define ADXL345_REG_DATAX1      0x33  // read only
#define ADXL345_REG_DATAY0      0x34  // read only
#define ADXL345_REG_DATAY1      0x35  // read only
#define ADXL345_REG_DATAZ0      0x36  // read only
#define ADXL345_REG_DATAZ1      0x37  // read only
#define ADXL345_REG_THRESH_ACT	0x24
#define ADXL345_REG_THRESH_INACT	0X25
#define ADXL345_REG_TIME_INACT 0x26
#define ADXL345_REG_ACT_INACT_CTL	0x27
#define ADXL345_REG_INT_ENABLE	0x2E


// Declare global variables needed to use the accelerometer
//volatile int * I2C0_ptr; // virtual address for I2C communication
//volatile int * SYSMGR_ptr; // virtual address for System Manager communication

#define SYSMGR_GENERALIO7 0x49C // 0xFFD0849C
#define SYSMGR_GENERALIO8 0x4A0
#define SYSMGR_I2C0USEFPGA 0x704



// Declare other variables and prototypes needed for a character device driver


/**************Variables*************/

void * I2C0_virtual; // virtual address for I2C communication
void * SYSMGR_virtual; // virtual address for System Manager communication

volatile unsigned int *I2C0_ENABLE;
volatile unsigned int *I2C0_ENABLE_STATUS;
volatile unsigned int *I2C0_CON;
volatile unsigned int *I2C0_TAR;
volatile unsigned int *I2C0_FS_SCL_LCNT;
volatile unsigned int *I2C0_DATA_CMD;
volatile unsigned int *I2C0_FS_SCL_HCNT;
volatile unsigned int *I2C0_RXFLR;

volatile unsigned int * I2C_ptr; //
volatile unsigned int * SYSMGR_GENERALIO7_ptr;
volatile unsigned int * SYSMGR_GENERALIO8_ptr;
volatile unsigned int * SYSMGR_I2C0USEFPGA_ptr;




/*************Prototipes***************/

void Pinmux_Config(void);
void I2C0_Init(void);
void ADXL345_Init(void);
void ADXL345_REG_WRITE(uint8_t address, uint8_t value);
void ADXL345_REG_READ(uint8_t address, uint8_t *value);
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len);
bool ADXL345_IsDataReady(void);
void ADXL345_XYZ_Read(int16_t szData16[3]);
void Int16toString( int16_t input1,int16_t input2,int16_t input3, unsigned char * output);

/************** Kernel character device driver /dev/accel. *************/
static int device_open (struct inode *, struct file *);
static int device_release (struct inode *, struct file *);
static ssize_t device_read (struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "accel"

static dev_t dev_no = 0;
static struct cdev *accel_cdev = NULL;
static struct class *accel_class = NULL;
#define MAX_SIZE 256 // assume that no message longer than this will be used
static char accel_msg[MAX_SIZE]; // the string that can be read or written

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write
};
/**************************************/

/* Code to initialize the accelerometer driver */
static int __init start_accel(void)
{

	
	// initialize the dev_t, cdev, and class data structures

	int err = 0;
	/* Get a device number. Get one minor number (0) */
	if ((err = alloc_chrdev_region (&dev_no, 0, 1, DEVICE_NAME)) < 0) {
		printk (KERN_ERR "accel: alloc_chrdev_region() device number error %d\n", err);
		return err;
	}
	accel_class = class_create (THIS_MODULE, DEVICE_NAME);
	
	// Allocate and initialize the char device
	accel_cdev = cdev_alloc ();
	accel_cdev->ops = &fops;
	accel_cdev->owner = THIS_MODULE;
	
	// Add the character device to the kernel
	if ((err = cdev_add (accel_cdev, dev_no, 1)) < 0) {
		printk (KERN_ERR "accel: cdev_add() char error %d\n", err);
		return err;
	}
	device_create (accel_class, NULL, dev_no, NULL, DEVICE_NAME );
	
	// generate virtual addresses
	I2C0_virtual = ioremap_nocache (0xFFC04000, 0x00000100);
	SYSMGR_virtual= ioremap_nocache (0xFFD08000, 0x00000800);
	
	if ((I2C0_virtual== 0) || (SYSMGR_virtual == NULL))
		printk (KERN_ERR "Error: ioremap_nocache returned NULL\n");
	
	
	Pinmux_Config ();
	I2C0_Init ();
	ADXL345_Init ();
	
	
	strcpy(accel_msg, "Acelerometro illoh\n");
	
	
	
	
	return 0;
}



static void __exit stop_accel(void)
{
	/* unmap the physical-to-virtual mappings */
	iounmap (I2C0_virtual);
	iounmap (SYSMGR_virtual);
	

	/* Remove the device from the kernel */
	device_destroy (accel_class, dev_no);
	cdev_del (accel_cdev);
	class_destroy (accel_class);
	unregister_chrdev_region (dev_no, 1);
	
	printk(KERN_INFO "My module is unloaded\n");
}

/* Called when a process opens accel */
static int device_open(struct inode *inode, struct file *file)
{
	return SUCCESS;
}

// Code for device_open and device_release
 static int device_release(struct inode *inode, struct file *file)
 {
	return 0;
 }
 
 /* Called when a process writes to accel. Stores the data received into
 * accel_msg, and returns the number of bytes stored. */
static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
	size_t bytes;
	bytes = length;
	
	if (bytes > MAX_SIZE - 1) // can copy all at once, or not?
		bytes = MAX_SIZE - 1;
	(void) copy_from_user (accel_msg, buffer, bytes);
	accel_msg[bytes] = '\0'; // NULL terminate
	// Note: we do NOT update *offset; we keep the last MAX_SIZE or fewer bytes
	return bytes;
}

// Functions needed to read/write registers in the ADXL345 device
//Read from device

void ADXL345_REG_READ(uint8_t address, uint8_t *value){

	
	// Send reg address (+0x400 to send START signal)
	*I2C0_DATA_CMD = address + 0x400;
	
	// Send read signal
	*I2C0_DATA_CMD = 0x100;

	// Read the response (first wait until RX buffer contains data)
	while (*I2C0_RXFLR == 0){}
	
	*value = *I2C0_DATA_CMD;
	
	
}

//Write in device

void ADXL345_REG_WRITE(uint8_t address, uint8_t value){

	// Send reg address (+0x400 to send START signal)
	*I2C0_DATA_CMD = address + 0x400;

	// Send value
	*I2C0_DATA_CMD = value;
}

//Multiread from device

void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len){

	// Send reg address (+0x400 to send START signal)
	*I2C0_DATA_CMD = address + 0x400;

	// Send read signal len times
	int i;
	for (i=0;i<len;i++)
		*I2C0_DATA_CMD = 0x100;

	// Read the bytes
	int nth_byte=0;
	while (len){
		if ((*I2C0_RXFLR) > 0){
			values[nth_byte] = *I2C0_DATA_CMD;
			nth_byte++;
			len--;
		}
	}
}

void Int16toString( int16_t input1,int16_t input2,int16_t input3, unsigned char * output){
	
	int16_t numero=input1;

	
	////////Obtener las unidades,decenas,centenas////////	
	int16_t millares;
	int16_t centenas;
	int16_t decenas;
	int16_t unidades;
	
	if (numero > 0){
		
		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		output[0]= ' ';
		output[1]= centenas +'0';
		output[2]= decenas +'0';
		output[3]= unidades +'0';

	}
	else{
		numero=numero*(-1);		

		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		

		output[0]='-';
		output[1]= centenas +'0';
		output[2]= decenas +'0';
		output[3]= unidades +'0';

	}


	
	numero=input2;

	////////Obtener las unidades,decenas,centenas////////	
	if (numero > 0){
		
		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		output[4]= ' ';
		output[5]= centenas +'0';
		output[6]= decenas +'0';
		output[7]= unidades +'0';

	}
	else{
		numero=numero*(-1);		

		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		output[4]='-';
		output[5]= centenas +'0';
		output[6]= decenas +'0';
		output[7]= unidades +'0';

	}

	
	////////Obtener las unidades,decenas,centenas////////	
	numero=input3;
	if (numero > 0){
		
		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		output[8]= ' ';
		output[9]= centenas +'0';
		output[10]= decenas +'0';
		output[11]= unidades +'0';

	}
	else{
		numero=numero*(-1);		

		centenas=(numero-(millares*1000))/100;
		decenas=(numero- (millares*1000 + centenas*100))/10;
		unidades=numero-(millares*1000 + centenas*100 + decenas*10 );

		

		output[8]='-';
		output[9]= centenas +'0';
		output[10]= decenas +'0';
		output[11]= unidades +'0';

	}
	
	output[12]= '\0';
	
}


static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	size_t bytes;
	uint8_t devid;
	int16_t mg_per_lsb = 4;
	int16_t XYZ[3];
	char XYZaxis[13] ;
	
	int16_t Xaxis_n;
	int16_t Yaxis_n;
	int16_t Zaxis_n;
	
	int n=0;
	
	if (ADXL345_IsDataReady()){
		ADXL345_XYZ_Read(XYZ);
		
		Xaxis_n= XYZ[0]*mg_per_lsb;
		Yaxis_n= XYZ[1]*mg_per_lsb;
		Zaxis_n= XYZ[2]*mg_per_lsb;
				
		//printf("X=%d mg, Y=%d mg, Z=%d mg\n", XYZ[0]*mg_per_lsb, XYZ[1]*mg_per_lsb, XYZ[2]*mg_per_lsb);
		Int16toString( Xaxis_n,Yaxis_n,Zaxis_n, XYZaxis);
				
		
		strcpy(accel_msg, XYZaxis);
		//strcpy(accel_msg, "1234\n");
		//printk(KERN_INFO "X=%d mg, Y=%d mg, Z=%d mg\n", XYZ[0]*mg_per_lsb, XYZ[1]*mg_per_lsb, XYZ[2]*mg_per_lsb);
		
		//strcpy(accel_msg, "Acelerometro illoh\n");
	}
	
	
	
	bytes = strlen (accel_msg) - (*offset); // how many bytes not yet sent?
	bytes = bytes > length ? length : bytes; // too much to send at once?
	
	if (bytes)
		(void) copy_to_user (buffer, &accel_msg[*offset], bytes);
	*offset = bytes; // keep track of number of bytes sent to the user
	return bytes;

}

void Pinmux_Config(void){

	//Le damos sus posiciones a cada componente
	SYSMGR_GENERALIO7_ptr=(unsigned int *) (SYSMGR_virtual+ SYSMGR_GENERALIO7);
	SYSMGR_GENERALIO8_ptr=(unsigned int *) (SYSMGR_virtual + SYSMGR_GENERALIO8 );
	SYSMGR_I2C0USEFPGA_ptr=(unsigned int *) (SYSMGR_virtual + SYSMGR_I2C0USEFPGA);
	
	*SYSMGR_I2C0USEFPGA_ptr=0;
	*SYSMGR_GENERALIO7_ptr=1;
	*SYSMGR_GENERALIO8_ptr=1;
		
	
	I2C0_ENABLE=(unsigned int *)(I2C0_virtual + 0x6C);
	I2C0_ENABLE_STATUS = (unsigned int *)(I2C0_virtual + 0x9C);
	I2C0_CON = (unsigned int *)(I2C0_virtual);
	I2C0_TAR = (unsigned int *)(I2C0_virtual +0x4);
	I2C0_FS_SCL_LCNT = (unsigned int *)(I2C0_virtual + 0x20);
	I2C0_DATA_CMD = (unsigned int *)(I2C0_virtual + 0x10);
	I2C0_FS_SCL_HCNT = (unsigned int *)(I2C0_virtual + 0x1C);
	I2C0_RXFLR = (unsigned int *)(I2C0_virtual + 0x78);
	
 
}

void I2C0_Init(void){
	
	// Abort any ongoing transmits and disable I2C0.
	*I2C0_ENABLE = 2;
	
	
	// Wait until I2C0 is disabled
	while(((*I2C0_ENABLE_STATUS)&0x1) == 1){}
	
	
	// Configure the config reg with the desired setting (act as
	// a master, use 7bit addressing, fast mode (400kb/s)).
	*I2C0_CON = 0x65;

	// Set target address (disable special commands, use 7bit addressing)
	*I2C0_TAR = 0x53;

	// Set SCL high/low counts (Assuming default 100MHZ clock input to I2C0 Controller).
	// The minimum SCL high period is 0.6us, and the minimum SCL low period is 1.3us,
	// However, the combined period must be 2.5us or greater, so add 0.3us to each.
	*I2C0_FS_SCL_HCNT = 60 + 30; // 0.6us + 0.3us
	*I2C0_FS_SCL_LCNT = 130 + 30; // 1.3us + 0.3us

	// Enable the controller
	*I2C0_ENABLE = 1;
	
	
	// Wait until controller is powered on
	while(((*I2C0_ENABLE_STATUS)&0x1) == 0){}
	
	
}

void ADXL345_Init(void){

	// +- 16g range, full resolution
	ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, XL345_RANGE_16G | XL345_FULL_RESOLUTION);

	// Output Data Rate: 200Hz
	ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, XL345_RATE_200);
	// The DATA_READY bit is not reliable. It is updated at a much higher rate than the Data Rate
	// Use the Activity and Inactivity interrupts as indicators for new data.
	ADXL345_REG_WRITE(ADXL345_REG_THRESH_ACT, 0x04); //activity threshold
	ADXL345_REG_WRITE(ADXL345_REG_THRESH_INACT, 0x02); //inactivity threshold
	ADXL345_REG_WRITE(ADXL345_REG_TIME_INACT, 0x02); //time for inactivity
	ADXL345_REG_WRITE(ADXL345_REG_ACT_INACT_CTL, 0xFF); //Enables AC coupling for thresholds
	ADXL345_REG_WRITE(ADXL345_REG_INT_ENABLE, XL345_ACTIVITY | XL345_INACTIVITY ); //enable interrupts

	// stop measure
	ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);

	// start measure
	ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);
}

bool ADXL345_IsDataReady(){
	bool bReady = false;
	uint8_t data8;
	
	//printk(KERN_ALERT "Antesread\n");
	
	ADXL345_REG_READ(ADXL345_REG_INT_SOURCE,&data8);
	if (data8 & XL345_ACTIVITY)
		bReady = true;

	return bReady;
}

void ADXL345_XYZ_Read(int16_t szData16[3]){

	uint8_t szData8[6];
	ADXL345_REG_MULTI_READ(0x32, (uint8_t *)&szData8, sizeof(szData8));

	szData16[0] = (szData8[1] << 8) | szData8[0];
	szData16[1] = (szData8[3] << 8) | szData8[2];
	szData16[2] = (szData8[5] << 8) | szData8[4];
}

MODULE_LICENSE("GPL");
module_init(start_accel);
module_exit(stop_accel);
