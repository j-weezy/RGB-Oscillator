/*
    Seg_Display.cpp

    Definition for Seg_Display class

    Writes to the display through a Serial-in Parallel-out (SIPO) shift register (74HC595). The constructor gets the pin numbers for the clock and data pins of the shift register and the four common pins for each of the digits on the display.

    Digits are set by passing a number in [0, 100) to set_digits().
    Numbers can be type float or int and will be parsed and displayed as XX.XX
    Note that this means that digits beyond tens and hundredths will not be shown.

    The digits of the segment display are made of 8 LEDs labeled A, B, C, D, E, F, G, DP and arranged like so:
       A
       _
    F |_|  B
    E |_|. C
       D  DP
    
    We send data to the display as a byte whose binary representation determines the state of each LED in this order {DP, A, B, C, D, E, F, G}
    For example 3 is represented as the byte 01111001
                _
    01111001 -> _|
                _|
    In the impementation, the digit images are defined in the get_byte_rep() function in hexadecimal.
*/

#include "Seg_Display.h"

const byte Seg_Display::decimal_point = 0;

Seg_Display::Seg_Display(uint8_t c_pin, uint8_t d_pin, uint8_t dig1, uint8_t dig2, uint8_t dig3, uint8_t dig4) {
    clock_pin = c_pin;
    data_pin = d_pin;
    digit_pin1 = dig1;
    digit_pin2 = dig2;
    digit_pin3 = dig3;
    digit_pin4 = dig4;
}

void Seg_Display::init() {
    // Initialize in setup()
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    pinMode(digit_pin1, OUTPUT);
    pinMode(digit_pin2, OUTPUT);
    pinMode(digit_pin3, OUTPUT);
    pinMode(digit_pin4, OUTPUT);

    current_digit = 0;
}

byte Seg_Display::get_tens(float raw) {
    // Need to cast to int for compatability with modulo operation
    int num = (int(raw) / 10) % 10;
    return byte(num);
}

byte Seg_Display::get_ones(float raw) {
    int num = int(raw) % 10;
    return byte(num);
}

byte Seg_Display::get_tenths(float raw) {
    int num = int(raw * 10) % 10;
    return byte(num);
}

byte Seg_Display::get_hundredths(float raw) {
    int num = int(raw * 100) % 10;
    return byte(num);
}

void Seg_Display::set_digits(float raw) {
    byte tens;
    byte ones;
    byte tenths;
    byte hundredths;
    tens = get_tens(raw);
    ones = get_ones(raw);
    tenths = get_tenths(raw);
    hundredths = get_hundredths(raw);
    // Set the digits right -> left
    display_digits[3] = get_byte_rep(hundredths);
    display_digits[2] = get_byte_rep(tenths);
    display_digits[1] = get_byte_rep(ones);
    // Set decimal point on ones place by setting the leftmost bit
    display_digits[1] += 0x80;
    display_digits[0] = get_byte_rep(tens);    
}

void Seg_Display::set_display() {
    display_off();
    disp(display_digits[current_digit]);
    switch (current_digit) {
        case 0:
            digitalWrite(digit_pin1, HIGH);
            break;
        case 1:
            digitalWrite(digit_pin2, HIGH);
            break;
        case 2:
            digitalWrite(digit_pin3, HIGH);
            break;
        case 3:
            digitalWrite(digit_pin4, HIGH);
            break;
    }
    current_digit = (current_digit + 1) % 4;
}

byte Seg_Display::get_byte_rep(uint8_t number) {
    // Legal inputs are between 0 and 10
    if ((number < 0) || (number > 9)) { return 0x00; }
    switch (number) {
        case 0:
            return 0x7E;
        case 1: 
            return 0x30;
        case 2: 
            return 0x6D;
        case 3:
            return 0x79;
        case 4:
            return 0x33;
        case 5:
            return 0x5B;
        case 6:
            return 0x5F;
        case 7:
            return 0x70;
        case 8:
            return 0x7F;
        case 9:
            return 0x7B;
    }
}

void Seg_Display::disp(byte data) {
    shiftOut(data_pin, clock_pin, LSBFIRST, data);
    digitalWrite(clock_pin, HIGH);
    digitalWrite(clock_pin, LOW);
}

void Seg_Display::display_off() {
    digitalWrite(digit_pin1, LOW);
    digitalWrite(digit_pin2, LOW);
    digitalWrite(digit_pin3, LOW);
    digitalWrite(digit_pin4, LOW);
}
