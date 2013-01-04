#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "avrutils.h"
#include "motor.h"

#ifndef NO_OF_MOTORS
	#error "'NO_OF_MOTORS' is not defined."
#endif

volatile uint8_t motor_i = 0;
extern ESC all_escs[NO_OF_MOTORS];

uint16_t start_time;
uint16_t end_time;

#define setCompare(time) \
	readCNT1(start_time); \
	end_time = start_time + time; \
	OCR1AH = end_time >> 8; \
	OCR1AL = end_time; \
    TIMSK1 |= (1 << OCIE1A);

uint8_t indexes[NO_OF_MOTORS];
uint8_t i,j; // general loop counters

extern void sort();

void setupEscs() {
	for (i = 0; i < NO_OF_MOTORS; ++i) {
		setDDRBit(all_escs[i].port, all_escs[i].pin);
	}
	//DDRA |= (1 << DDA7);
	//DDRB |= (1 << DDB0);
}


static void two_second_constant(uint8_t drive_value) {
	for (i = 0; i < NO_OF_MOTORS; ++i) {
		all_escs[i].drive = drive_value;
	}
	for (i = 0; i < 100; ++i) {
        serviceEscs();
    }
}

void calibrateEscs() {
	two_second_constant(MAX_SPEED);
    two_second_constant(MAX_SPEED);
    two_second_constant(0);
    two_second_constant(0);
    two_second_constant(MAX_SPEED/2);
    two_second_constant(MAX_SPEED/2);
    two_second_constant(MAX_SPEED/2);
    two_second_constant(MAX_SPEED/2);
}

void serviceEscs() {
	sort();
	motor_i = 0;
	for (j = 0; j < NO_OF_MOTORS; ++j) {
		setBit(all_escs[j].port, all_escs[j].pin);
	}
	setCompare(INT16_C(1000/8) + (all_escs[indexes[0]].drive >> 1));
	_delay_ms(20);
}

ESC* esc;
ESC* esc2;
uint16_t diff;
ISR(TIM1_COMPA_vect) 
{
	static ESC* esc;
	static ESC* esc2;
	static uint16_t diff;
	esc = &all_escs[indexes[motor_i++]];
	clearBit(esc->port, esc->pin);
	while (motor_i < NO_OF_MOTORS) {
		esc2 = &all_escs[indexes[motor_i]];
		diff = (esc2->drive - esc->drive) >> 1;
		if (diff == 0) {
			// turn this one off too and check the next
			clearBit(esc2->port, esc2->pin);
			esc = esc2;
			++motor_i;
		} else {
			setCompare(diff);
			break;
		}
	}
	if (motor_i == NO_OF_MOTORS) {
		TIMSK1 &= ~(1 << OCIE1A);
	}
}

