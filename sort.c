#include <stdint.h>
#include "motor.h"

extern uint8_t indexes[NO_OF_MOTORS];
extern ESC* all_escs;

void sort() {
	for (uint8_t i = 0; i < NO_OF_MOTORS; ++i) {
		ESC* esc = &all_escs[i];
		uint8_t hole = i;
		while (hole > 0 && all_escs[indexes[hole-1]].drive > esc->drive) {
			indexes[hole] = indexes[hole-1];
			--hole;
		}
		indexes[hole] = i;
	}
}