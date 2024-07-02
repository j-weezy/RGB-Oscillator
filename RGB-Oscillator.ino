/*
  RGB-Oscillator.ino

  Meant to be a demonstration of oscillations modelled after Simple Harmonic Motion.
  Uses a sine function to control two RGB LEDs so they oscillate between red and blue and bright and dim.
  y(t) = A*sin(2*pi*f*t + h)
  For example, one full period would be between two occurances of max brightness red.
  To control the timing, the oscillator must be run in the loop with a single pre-determined delay() call: delay_time in the Oscillator class definition.

  The oscillators are interactive via incrementing their periods and the phase constant between them.

  Interaction with the oscillators is done via a rotary encoder (KY-040).
  Pressing the encoder will toggle between:
    None
    Oscillator1 Period
    Oscillator2 Period
    Phase constant
  The parameters are then incremented by rotating the knob of the encoder.

  A 7-Segment 4-Digit display is used to display the selected parameter.
  This implementation uses a common cathode display driven by a SIPO shift register (74HC595).
  The common pins for the digits are connected to ground via NPN transistors, whose base pins are driven by the digit pins on the arduino.

  To handle continuously updating the display, the Timer1 overflow interrupt is used.
  When Timer1 overflows, the display is updated via an ISR() function. This prevents the oscillators from being affected by updating the display.
*/

#include "Oscillator.h"
#include "Seg_Display.h"
#include <Arduino.h> // Included to get Intellisense to work

// Define pins on Arduino
#define DATA_PIN 0 // DS [S1] on 74HC595
#define CLOCK_PIN 2 // SH_CP [SCK] and ST_CP [RCK] on 74HC595
#define BLUE 3 // PWM
#define PIN_A 4 // Connected to DT on KY-040
#define RED 5 // PWM
#define BLUE2 6 // PWM
#define DIG_1 7 // Leftmost digit on display
#define DIG_2 8
#define BUTTON_PIN 9 // Connected to SW on KY-040
#define PIN_B 10 // Connected to CLK on KY-040
#define RED2 11 // PWM
#define DIG_3 12
#define DIG_4 13 // Rightmost digit on display

// Initialize global variables
const float A_max = 255; // A

// RGB values defined globally for persistence between loops
int blue_value = 0;
int red_value = 0;
int blue2_value = 0;
int red2_value = 0;

// Parameters for rotary encoder KY-040
int A_val;
int B_val;
int button_toggle = 0;
/*
  0 -> None
  1 -> Freq1
  2 -> Freq2
  3 -> Phi
*/
const int toggle_max = 4; // Set the number of states

Oscillator oscillator1 = Oscillator(2); // y1 = sin(2*pi*(1/2)*t)
Oscillator oscillator2 = Oscillator(2, 2); // y2 = sin(2*pi*(1/2)t + 2*(pi/12))

Seg_Display display = Seg_Display(CLOCK_PIN, DATA_PIN, DIG_1, DIG_2, DIG_3, DIG_4);

void setup();
void loop();
void handle_display();

void setup() {
  // Pins for Oscillator RGB LEDs
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(BLUE2, OUTPUT);
  
  // Pins for Rotary Encoder
  pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  display.init();
  display.display_off();

  // Setup Timer1
  TCCR1A = 0; // Set Timer/Counter Control registers
  TCCR1B = 0;
  TCCR1B = 1; // Set prescaler
  TCNT1 = 0; // Set Timer/Counter 1 register
  TIMSK1 |= (1 << TOIE1); // Set Timer Interrupt Mask register

  sei();
}

void loop() {
  // Handle button press
  if (digitalRead(BUTTON_PIN) == 0) {
    button_toggle += 1;
    button_toggle = button_toggle % toggle_max; // Ensure button_toggle < toggle_max
    // Delay for debounce
    delay(200);
  }  
  
  // Handle rotary encoder
  A_val = digitalRead(PIN_A);
  B_val = digitalRead(PIN_B);

  /*
    The idle state of the pins is 1 (HIGH).
    When A -> 0, the encoder is being rotated.
    If B = 1, Pin B hasn't changed yet, and the encoder is rotating clockwise.
    If B = 0, Pin B changed before Pin A, and the encoder is rotating counter-clockwise.

    Note the conventions of CW and CCW are depended on which pins (CLK and DT) are called A and B.
    So the directions are distinct, i.e. you can always tell if the encoder is being rotated in a different direction,
      but the directions of CW and CCW may or may not be reversed.

    To avoid noise from the sensor, we will make sure to include a delay of at least 100ms.
  */
  if (button_toggle == 1) { // Oscillator1 period
    if (A_val == 0) {
      // Clockwise -> increment
      if (B_val == 1) { oscillator1.increment_period(); }
      else { oscillator1.decrement_period(); }
      delay(100); // Debounce
    }
  }
  else if (button_toggle == 2) { // Oscillator2 period
    if (A_val == 0) {
      // Clockwise -> increment
      if (B_val == 1) { oscillator2.increment_period(); }
      else { oscillator2.decrement_period(); }
      delay(100); // Debounce
    }
  }
  else if (button_toggle == 3) { // Phi
    if (A_val == 0) {
      if (B_val == 1) { oscillator2.increment_phi(); }
      else { oscillator2.decrement_phi(); }
      oscillator1.reset_param();
      oscillator2.reset_param();
      delay(100); // Debounce
    }
  }

  // Set oscillator1
  float raw = A_max * oscillator1.state();

  blue_value = 0;
  red_value = 0;
  
  if (oscillator1.state() < 0) { blue_value = int(-1 * raw); }
  if (oscillator1.state() > 0) { red_value = int(raw); }

  // Set oscillator2
  float raw2 = A_max * oscillator2.state();

  blue2_value = 0;
  red2_value = 0;

  if (oscillator2.state() < 0) { blue2_value = int(-1 * raw2); }
  if (oscillator2.state() > 0) { red2_value = int(raw2); }

  analogWrite(BLUE, blue_value);
  analogWrite(RED, red_value);
  analogWrite(BLUE2, blue2_value);
  analogWrite(RED2, red2_value);

  oscillator1.update();
  oscillator2.update();
  delay(Oscillator::delay_time);
}

void handle_display() {
  if (button_toggle == 0) { 
    display.display_off();
    return;
  }
  else if (button_toggle == 1) { display.set_digits(oscillator1.get_period()); }
  else if (button_toggle == 2) { display.set_digits(oscillator2.get_period()); }
  else if (button_toggle == 3) { display.set_digits(oscillator2.get_phi()); }
  display.display_off(); // turn off the display
  display.set_display();
}

ISR(TIMER1_OVF_vect) { // Timer1 interrupt service routine (ISR)
  handle_display();
}
