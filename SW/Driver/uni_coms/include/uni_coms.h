
#ifndef UNI_COMS_H
#define UNI_COMS_H

// For uint16_t.
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif


#define DEV_NAME "uni_coms"
#define DEV_FN "/dev/uni_coms"


#include <linux/ioctl.h> // For _IOW and _IORW

typedef struct {
	uint8_t pin;
	unsigned char data;
} uart_coms__ioctl_data;

//#define MOTOR_CLTR__N_SERVO 6
//#define MOTOR_CLTR__N_BLDC 1

/*
typedef struct {
	int16_t pos_fb[MOTOR_CLTR__N_SERVO];
	int64_t pulse_cnt_fb[MOTOR_CLTR__N_BLDC];
} uart_coms__read_arg_fb_t;
*/

#define IOCTL_WRITE\
	_IOW('w', 1, uart_coms__ioctl_data)
	
#define IOCTL_READ\
	_IOW('r', 0, uart_coms__ioctl_data)


#endif // UNI_COMS_H
