
#include <linux/unistd.h>
#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()
#include <asm/io.h>
#include <linux/delay.h>


#define UART_COMS__N 2
#define BAUD_RATE 9600

MODULE_LICENSE("Dual BSD/GPL");

#include "include/uni_coms.h"
#include "gpio.h"

#define TX 14
#define RX 15


void send_8bit_serial_data(uint8_t data_tx, uint8_t mask){


	if(data_tx & mask){
	
		gpio__set(TX);
	
	}
	else {
		gpio__clear(TX);

	}
	
 

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
	uint8_t mask = 0x80;
	int i;
	uint8_t podatak = 'Z';
	
	gpio__clear(TX);
	msleep(100);
	for(i = 0; i < 7; i++){
		send_8bit_serial_data(podatak,mask);
		msleep(100);

		mask >>= 1;
	}

	gpio__set(TX);
	return sizeof(podatak);
}


static ssize_t uart_coms_read(
	struct file* filp,
	char* buf,
	size_t len,
	loff_t* f_pos
) {
	//char mask = 0x80;
	uint16_t karakter=0x00;
	int i;
	//while(gpio__read(RX));
	
	while(gpio__read(RX));
	
	msleep(50);
	
	for(i = 0; i < 7; i++){
		msleep(100);
		printk(KERN_INFO ":-----(0x%02x)---------- ", karakter);
		karakter = karakter | gpio__read(RX);
		karakter <<=1;
		printk(KERN_INFO ":-----(0x%02x)---------- ", karakter);
	}
	
	
	
	//gpio__set(RX);
	if(copy_to_user(buf,(uint16_t*)&karakter, len )){
		
		
		return -EFAULT;
	}else{
		printk(KERN_INFO ":-----%s---------- \n", buf);
		printk(KERN_INFO ":-----(0x%02x)---------- \n", len);
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

	gpio__steer_pinmux(TX, GPIO__OUT);
	gpio__steer_pinmux(RX, GPIO__IN);

	
exit:
	if(r){
		printk(KERN_ERR DEV_NAME": %s() failed with %d!\n", __func__, r);
		uart_coms_exit();
	}else{
		printk(KERN_INFO DEV_NAME": Inserting module successful.\n");
	}
	return r;
}


module_init(uart_coms_init);
module_exit(uart_coms_exit);
