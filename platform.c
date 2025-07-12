#include <xc.h>
#include "platform.h"

void INIT_PINS() {
    // Set LEDs pins
    TRISA2 = 0;
    LATA2 = 1; // Blue
    
    TRISA3 = 0;
    LATA3 = 0; // Red
    
    TRISA4 = 0;
    LATA4 = 0; // Green
    
    // Set up CAN TX
    TRISC1 = 0;
    RC1PPS = 0x33;

    // Set up CAN RX
    TRISC0 = 1;
    ANSELC0 = 0;
    CANRXPPS = 0b00010000;
}

// need to test
void SET_BLUE_LED(int state) {
    if (state == 0 || state == 1)
        LATA2 = state;
}

void SET_RED_LED(int state) {
    if (state == 0 || state == 1)
        LATA3 = state;
}

void SET_GREEN_LED(int state) {
    if (state == 0 || state == 1)
        LATA4 = state;
}

// for heartbeats we want to alternate,
// manually setting true false is annoying
// test locally in C first probs
void TOGGLE_BLUE_LED() {
    LATA2 ^ 1;
}

void TOGGLE_RED_LED() {
    LATA3 ^ 1;
}

void TOGGLE_GREEN_LED() {
    LATA4 ^ 1;
}