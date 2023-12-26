#ifndef UART_COMS_H
#define UART_COMS_H

#include <linux/types.h>

#define UART_COMS__N 2
#define BAUD_RATE 115200


typedef enum {
    UART_COMS__CH_0,  //GPIO14, pin 8
    UART_COMS__CH_1,  //GPIO15, pin 10
} uart_coms__ch_t;

int uart_coms__init(void);
void uart_coms__exit(void);

void uart_coms__set_sinhro(uart_coms__ch_t ch, uint32_t sinhro);

void uart_coms__set_message(uart_coms__ch_t ch, uint32_t msg)

#endif // UART_COMS_H