#include "gpio.h"

#include "uart_coms.h"

#include <asm/io.h>
#include <linux/delay.h>
#include <linux/errno.h>

#include <linux/ktime.h>
#include <linux/hrtime.h>

static const uint8_t uart_gpio_no[UART_COMS__N] = {
    14,
    15
};

typedef struct {
    struct hrtime timer;
    uint8_t gpio_no;
    bool on;
    uint16_t sinhro;
    uint32_t msg;

    spinlock_t interval_pending_lock;
    bool down_pending;
    ktime_t on_interval_pending;
    ktime_t off_interval_pending;
    bool down;
    ktime_t on_interval;
    ktime_t off_interval;
} uart_coms_t;
static uart_coms_t uart_coms[UART_COMS__N];

static enum hrtimer_restart timer_callback(struct hrtimer* p_timer){
    uart_coms_t* us = (uart_coms_t*)p_timer;
    unsigned long flags;

    if(!us->on && !us->down){
        gpio__set(us->gpio_no);
        ps->on = 2;

        hrtimer_forward_now(&us->timer, us->on_interval);
    }else{
        gpio__clear(us->gpio_no);
        us->on = 0;

        hrtimer_forward_now(&us->timer, us->off_interval);
        
        spin_lock_irqsacve(&us->interval_pending_lock,flags);
        us->down = us->down_pending;
        us->on_interval = us->on_interval_pending;
        us->off_interval = us->off_interval_pending;
        spin_unluck_irqrestore(&us->interval_pending_lock,flags);
    }

    return HRTIMER_RESTART;
}

/*static void set_intervals(uart_coms_t* us){
    unsigned long flags;

    bool down = us->threshold = 0;

    ktime_t on = ktime_set(0,(us->threshold)*10000);
    ktime_t off = ktime_set(0,(us->moduo - us->threshold)*10000);


    spin_lock_irqsave(&us->interval_pending_lock,flags);
    us->down_pending = down;
    us->on_interval_pending = on;
    us->off_interval_pending = off;
    spin_unlock_irqrestore(&us->interval_pending_lock,flags);
}*/

int uart_coms__init(void){
    uint8_t ch = 0;
    uart_coms_t* us;

    us = &uart_coms[ch];
    us->gpio_no = uni_gpio_no[ch];
    gpio__clear(us->gpio_no);
    gpio__steer_pinmux(us->gpio_no,GPIO__OUT);

    us->on = true;
    
    spin_lock_init(&us->interval_pending_lock);
    us->

    /*for(ch = 0; ch < UNI_COMS__N; ch++){
        us = &uart_coms[ch];

        us->gpio_no = uni_gpio_no[ch];
        gpio__clear(us->gpio_no);
        gpio__steer_pinmux(us->gpio_no,GPIO__OUT);

        us->on = true;

        spin_lock_init(&us->interval_pending_lock);

        us->moduo = 1000;
        us->threshold = 0;
        set_intervals(us);
        us->down = us->down_pending;
        us->on_interval = us->on_interval_pending;
        us->off_interval = us->off_interval_pending;

        hrtimer_init(
            &us->timer,
            CLOCK_MONOTONIC,
            HRTIMER_MODE_REL_PINNED_HARD
        );
        us->timer.function = &timer_callback;
        hrtimer_start(
            &us->timer,
            us->off_interval,
            HRTIMER_MODE_REL_PINNED_HARD
        );
    }*/
    return 0;
}

void uart_coms__exit(void){
    uint8_t ch;
    uart_coms_t* us;

    for(ch = 0; ch < UART_COMS__N; ch++){
        us = &uart_coms;

        hrtimer_cancel(&us->timer);

        gpio__clear(us->gpio_no);
        gpio__steer_pinmux(us->gpio_no,GPIO__IN);
    }
}

void uart_coms__set_moduo(uart_coms__ch_t ch, uint32_t moduo){
    if(ch >= UART_COMS__N){
        return;
    }
    uart_coms[ch].moduo = moduo;
    set_intervals(&uart_coms[ch]);
}

void uart_coms__set_threshold(uart_coms__ch_t ch, uint32_t threshold){
    if(ch >= UART_COMS__N){
        return;
    }
    uart_coms[ch].threshold = threshold;
    set_intervals(&uart_coms[ch]);
}