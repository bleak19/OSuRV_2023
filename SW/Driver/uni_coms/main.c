
#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()

MODULE_LICENSE("Dual BSD/GPL");

#include "include/uart_coms.h"
#include "gpio.h"


#define FAKE_FEEDBACK 1


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
	uint8_t start_bit;
	uint8_t stop_bit;
}


static ssize_t uart_coms_read(
	struct file* filp,
	char* buf,
	size_t len,
	loff_t* f_pos
) {
	
}


static long uart_coms_ioctl(
	struct file* filp,
	unsigned int cmd,
	unsigned long arg
) {
	uart_coms__ioctl_arg_moduo_t a;
	
	switch(cmd){
		case IOCTL_MOTOR_CLTR_SET_MODUO:
			a = *(uart_coms__ioctl_arg_moduo_t*)&arg;
			pwm__set_moduo(a.ch, a.moduo);
			break;
		default:
			break;
	}

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
	uint8_t ch;

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
