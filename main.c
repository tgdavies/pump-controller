#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "avrutils.h"
#include "motor.h"



ESC all_escs[1];
/*
 * Connections:
 *      ESC: PB0 (JP2)
 *      UP BUTTON: PA7 (JP1)
 *      DOWN BUTTON: PA3 (JP5)
 *      SET ENABLE: PB2 (JP4) (active low)
 */
#define SET_ENABLE (PINB_byte.b2)
#define DOWN_BUTTON (PINA_byte.b3)
#define UP_BUTTON (PINA_byte.b7)

#define DRIVE_EEPROM_LOC ((const uint8_t*)0)

uint8_t EEMEM drive_storage = STOP_SPEED;

typedef struct _button {
    uint8_t (*read)();
    uint8_t state;
    uint8_t changed;
    uint8_t count;
} BUTTON;

static uint8_t read_up_button() {
    return !UP_BUTTON;
}

static uint8_t read_down_button() {
    return !DOWN_BUTTON;
}

static uint8_t read_enable_button() {
    return !SET_ENABLE;
}

BUTTON up_button = { .read = read_up_button };
BUTTON down_button = { .read = read_down_button };
BUTTON enable_button = { .read = read_enable_button };

#define CHECK_MS        (20)
#define DEBOUNCE_MS     (100)

static void debounce(BUTTON* button) {
    static uint8_t raw;
    raw = button->read();
    button->changed = 0;
    if (raw == button->state) {
        button->count = DEBOUNCE_MS / CHECK_MS;
    } else {
        if (button->count-- == 0) {
            button->state = raw;
            button->changed = 1;
            button->count = DEBOUNCE_MS / CHECK_MS;
        }
    }
}

static uint8_t button_pressed(BUTTON* button) {
    return button->changed && button->state;
}

int main(void) {
    // set the clock prescaler to 1 to get us an 8MHz clock -- the CKDIV8 fuse is programmed by default, so initial prescaler is /8
    CLKPR = 0x80;
    CLKPR = 0x00;

    all_escs[0].port = B;
    all_escs[0].pin = PB0;
    PORTA_byte.val = 0;
    PORTB_byte.val = 0;
    setup_leds();
    red(1);
    green(1);
    TCCR1B = 0x03; // CK/64, i.e. 125KHz (0.008ms), or 488Hz MSB or 1.9Hz overflow.
    // initialise the ESC with a central setting

    setupEscs();
    red(0);
    sei();
    calibrateEscs();
    green(0);
    red(1);
    static uint8_t drive;
    drive = eeprom_read_byte(&drive_storage);
    static uint8_t count = 0;
    for (;;) {
        ++count;
        serviceEscs();
        debounce(&up_button);
        debounce(&down_button);
        debounce(&enable_button);
        if (enable_button.state) {
            if (button_pressed(&down_button) && drive > STOP_SPEED) {
                drive--;
            }
            if (button_pressed(&up_button) && drive < MAX_SPEED) {
                drive++;
            }
            green((count >> 5) & 0x01); // flash green to show we are in set mode
            red(drive == STOP_SPEED || drive == MAX_SPEED);
        }
        all_escs[0].drive = drive;
    }
    return 0; /* never reached */
}

