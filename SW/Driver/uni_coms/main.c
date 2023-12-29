
#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/hrtime.h>

#define UART_COMS__N 2
#define BAUD_RATE 9600

MODULE_LICENSE("Dual BSD/GPL");

#include "include/uart_coms.h"
#include "gpio.h"


#define FAKE_FEEDBACK 1

static const uint8_t uart_gpio_no[UART_COMS__N] = {
    14, //tx
    15  //rx
};

int send_8bit_serial_data(char data_tx, char mask){


	if(data_tx & mask){
	
		gpio__set(uart_gpio_no[0]);
	
	}
	else {
		gpio__clear(uart_gpio_no[0]);

	}
	
 

data_tx = data_tx<< 1;
//delay(1/BAUD_RATE);
}
 //send stop bit



uint8_t tx_data;
uint8_t rx_data;

static int uart_coms_open(struct inode *inode, struct file *filp) {
	return 0;
}

static int uart_coms_release(struct inode *inode, struct file *filp) {
	return 0;
}

static ssize_t uart_coms_write(
	struct file* filp,
	const char *buf,
	size_t len,
	loff_t *f_pos
) {
	gpio__clear(uart_gpio_no[0]);
	//delay(1/BAUD_RATE);
	delay(100);
	char podatak = 'h';
	char mask
	int i;
	for(i = 0; i < 8; i++){
		send_8bit_serial_data(podatak);
		delay(100);

		mask >>= 1;
	}

	gpio__set(uart_gpio_no[0]);
	return sizeof(podatak);
}


static ssize_t uart_coms_read(
	struct file* filp,
	char* buf,
	size_t len,
	loff_t* f_pos
) {
	mask = 0x80;
	char karakter=0x00;
	while(gpio__read(uart_gpio_no[1]));
	delay(50);
	for(i = 0; i < 8; i++){
		delay(100);
		karakter = gpio__read(uart_gpio_no[1]);
		karakter <<=1;
	}
	gpio__set(uart_gpio_no[1]);
	if(copy_to_user(buf,(uint8_t*)&karakter, len )){
		return -EFAULT;
	}else{

		return len;
	}
}

uart_coms__ioctl_data data;



static long uart_coms_ioctl(
	struct file* filp,
	unsigned int cmd,
	unsigned long arg
) {
	/*uart_coms__ioctl_data a;
	pthread_t write_t;
	pthread_t read_t;
	
	//pthread_mutex_init(&cs_mutex, NULL);
	
	switch(cmd){
		case IOCTL_WRITE:
			pthread_create(&write_t, NULL,  send_8bit_serial_data, data);
			
			break;
		case IOCTL_READ:
			
			break;
		default:
			break;
	}
	*/
	return 0;
}



loff_t uart_coms_llseek(
	struct file* filp,
	loff_t offset,
	int whence
) {
	switch(whence){
		case SEEK_SET:
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			filp->f_pos += offset;
			break;
		case SEEK_END:
			return -ENOSYS; // Function not implemented.
		default:
			return -EINVAL;
		}
	return filp->f_pos;
}

static struct file_operations uart_coms_fops = {
	open           : uart_coms_open,
	release        : uart_coms_release,
	read           : uart_coms_read,
	write          : uart_coms_write,
	unlocked_ioctl : uart_coms_ioctl,
	llseek         : uart_coms_llseek
};


void uart_coms_exit(void) {
	gpio__exit();
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	
	printk(KERN_INFO DEV_NAME": Module removed.\n");
}

int uart_coms_init(void) {
	int r;
	//uint8_t ch;

	printk(KERN_INFO DEV_NAME": Inserting module...\n");
	
	r = register_chrdev(DEV_MAJOR, DEV_NAME, &uart_coms_fops);
	if(r < 0){
		printk(KERN_ERR DEV_NAME": cannot obtain major number %d!\n", DEV_MAJOR);
		goto exit;
	}

	r = gpio__init();
	if(r){
		goto exit;
	}

	gpio__steer_pinmux(uart_gpio_no[0], GPIO__OUT);
	gpio__steer_pinmux(uart_gpio_no[1], GPIO__IN);

	
exit:
	if(r){
		printk(KERN_ERR DEV_NAME": %s() failed with %d!\n", __func__, r);
		uni_coms_exit();
	}else{
		printk(KERN_INFO DEV_NAME": Inserting module successful.\n");
	}
	return r;
}


module_init(uart_coms_init);
module_exit(uart_coms_exit);
