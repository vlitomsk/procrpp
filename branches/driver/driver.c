#include <avr/io.h>
#ifndef _AVR_IOM8_H_
#include <avr/iom8.h>
#endif

#define F_CPU 8000000UL 
#include <util/delay.h>

// все периоды в миллисекундах
#define BELOW_PEAKS 16

#define NEUTRAL_TIME 1.4
#define GAS_TIME 1.85
#define GAS_NF 1.5
#define STOP_TIME 1


#define GASSTOP_PIN 3

#define put_signal(pick_length, pd_out) { \
    PORTD &= (0xff ^ (1 << pd_out)); \
    _delay_ms(BELOW_PEAKS); \
    PORTD |= (1 << pd_out); \
    _delay_ms(pick_length); \
}

int __i;

/* time_ms - сколько примерно миллисекунд пульсировать ступеньками с пиком в signal_time_ms */
#define play_signal(time_ms, signal_time_ms) { \
    int iters = time_ms / (signal_time_ms + BELOW_PEAKS); \
    for (__i = 0; __i < iters; __i++) \
        put_signal(signal_time_ms, GASSTOP_PIN); \
}

#define GAS_INIT_MS 5100
#define STOP_INIT_MS 3100
#define NEUTRAL_INIT_MS 3100

#define IDLE_INIT_MS 7000

void main(void) {
    DDRD = 0xFF;
    ACSR = 0x80;
   
    PORTD = 0xff;
    play_signal(GAS_INIT_MS, GAS_TIME);
    play_signal(STOP_INIT_MS, STOP_TIME);
    play_signal(NEUTRAL_INIT_MS, NEUTRAL_TIME);
    play_signal(IDLE_INIT_MS, NEUTRAL_TIME);

    while (1) {
        play_signal(1000, GAS_NF); // примерно 1с газовать не на полную
        play_signal(2000, NEUTRAL_TIME); // примерно 2c - нейтраль
    }    
}
