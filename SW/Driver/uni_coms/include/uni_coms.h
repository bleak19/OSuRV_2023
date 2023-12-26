
#ifndef UART_COMS_H
#define UART_COMS_H

// For uint16_t.
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif


#define DEV_NAME "uart_coms"
#define DEV_FN "/dev/uart_coms"

#define WIDE_PWM 1


#include <linux/ioctl.h> // For _IOW and _IORW

typedef struct {
	uint8_t ch;
	uint16_t moduo;
} uart_coms__ioctl_arg_moduo_t;

//#define MOTOR_CLTR__N_SERVO 6
//#define MOTOR_CLTR__N_BLDC 1

typedef struct {
	int16_t pos_fb[MOTOR_CLTR__N_SERVO];
	int64_t pulse_cnt_fb[MOTOR_CLTR__N_BLDC];
} uart_coms__read_arg_fb_t;


#define IOCTL_MOTOR_CLTR_SET_MODUO \
	_IOW('s', 0, uart_coms__ioctl_arg_moduo_t)


#endif // UART_COMS_H
