
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

void print_binary(uint8_t data);


void send_8bit_serial_data(uint8_t data_tx, uint8_t mask){


	if(data_tx & mask){
	
		gpio__set(TX);
	
	}
	else {
		gpio__clear(TX);

	}
	
	//printk(KERN_INFO	":--------------- WRITE TX------------");
	//print_binary(gpio__read(TX));
	
 

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

//debugging func
void print_binary(uint8_t data){
	int i;
	for(i = 7; i>=0; i--) {
			printk(KERN_INFO "%d", (data >> i) &1);	
	}
	
	printk(KERN_INFO "\n");
}

static ssize_t uart_coms_write(
	struct file* filp,
	const char *buf,
	size_t len,
	loff_t *f_pos
) {
	uint8_t mask = 0b10000000;
	int i;
	uint8_t podatak = 'Q';
	
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
	
	
	uint8_t mask=0b00000001;
	uint8_t data = 0b00000000;
	int i;
	
	
	while(gpio__read(RX));
	
	msleep(50);
	
	for(i = 0; i < 8; i++){
		msleep(100);
		//used for debugging
		//printk(KERN_INFO ":--------------- Read RX");
		//printk(KERN_INFO ":--------------- Read RX VALUE %d", gpio__read(RX));
		
		data+= mask & gpio__read(RX);
		
		if(i!=7){
			data <<=1;
		}
	}
		//used for debugging 
		//printk(KERN_INFO ":-----(0x%02x)-------%c--- Read", karakter,karakter);
		//print_binary(data);
	
	
	gpio__set(RX); //end signal
	if(copy_to_user(buf,(uint8_t*)&data, len )){
		
		
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
