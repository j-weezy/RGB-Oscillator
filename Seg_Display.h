/*
    Seg_Display.h

    Declaration of Seg_Display class for handling 7-Segment 4-Digit Display with shift register.
    Class will handle pushing display data to the register and needs access to number to be displayed and a toggle.
*/

#include <Arduino.h>

class Seg_Display {
    private:
        byte display_digits[4];
        byte digits[4];
        uint8_t data_pin;
        uint8_t clock_pin;
        uint8_t digit_pin1;
        uint8_t digit_pin2;
        uint8_t digit_pin3;
        uint8_t digit_pin4;
        uint8_t current_digit;
        static const byte decimal_point;
        byte get_tens(float raw);
        byte get_ones(float raw);
        byte get_tenths(float raw);
        byte get_hundredths(float raw);
        byte get_byte_rep(uint8_t number);
        void disp(byte data);

    public:
        Seg_Display(uint8_t c_pin, uint8_t d_pin, uint8_t dig1, uint8_t dig2, uint8_t dig3, uint8_t dig4);
        void init();
        void set_digits(float raw);
        void set_display();
        void display_off();
};