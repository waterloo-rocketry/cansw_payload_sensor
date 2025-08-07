#include <xc.h>
#include "platform.h"

void INIT_PINS() {
    // Set up CAN TX
    TRISC1 = 0;
    RC1PPS = 0x33;

    // Set up CAN RX
    TRISC0 = 1;
    ANSELC0 = 0;
    CANRXPPS = 0b00010000;
    
    TRISA2 = 0; // Blue LED output enable
    TRISA3 = 0; // Green LED output enable
    TRISA4 = 0; // Red LED output enable
    
    TRISA5 = 0;
}

void SET_BLUE_LED(int state) {
    if (state == 0 || state == 1)
        LATA2 = state;
}

void SET_GREEN_LED(int state) {
    if (state == 0 || state == 1)
        LATA3 = state;
}

void SET_RED_LED(int state) {
    if (state == 0 || state == 1)
        LATA4 = state;
}

// for heartbeats we want to alternate,
// manually setting true false is annoying
void TOGGLE_BLUE_LED() {
    LATA2 ^= 1;
}

void TOGGLE_GREEN_LED() {
    LATA3 ^= 1;
}

void TOGGLE_RED_LED() {
    LATA4 ^= 1;
}