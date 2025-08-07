/* 
 * File:   platform.h
 * Author: cstubbs
 *
 * Created on May 27, 2025, 9:48 PM
 */

#ifndef PLATFORM_H
#define	PLATFORM_H

void INIT_PINS();

// Functions for turning on or off leds
void SET_BLUE_LED(int state);
void SET_RED_LED(int state);
void SET_GREEN_LED(int state);

void TOGGLE_BLUE_LED();
void TOGGLE_RED_LED();
void TOGGLE_GREEN_LED();


#endif	/* PLATFORM_H */

