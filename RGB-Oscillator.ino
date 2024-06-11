/*
  sine_test.ino

  Test for using sin() built-in to build oscillator.
  We would like to be able to adjust the parameters in the loop with input peripherals like buttons and/or knobs.

  For this, we will use a rotary encoder.
  Turning the encoder will increase and decrease the parameter.
  Pressing the encoder will toggle between parameters.
*/

#include "Oscillator.h"

// Define pins on Arduino
#define BLUE 3
#define RED 5
#define BUTTON_PIN 8
#define BLUE2 6
#define DATA_PIN 7 // DS [S1] on 74HC595
#define RED2 9
#define LATCH_PIN 10 // ST_CP [RCK] on 74HC595
#define CLOCK_PIN 11 // SH_CP [SCK] on 74HC595
#define PIN_A 13 // Connected to DT on KY-040
#define PIN_B 12 // Connected to CLK on KY-040

// Initialize global variables

int data_pin = 7;
int latch_pin = 10;
int clock_pin = 11;

const float A_max = 255; // A

int blue_value = 0;
int red_value = 0;
int blue2_value = 0;
int red2_value = 0;

// Parameters for rotary encoder KY-040
int A_val;
int B_val;
int button_toggle = 0;
const int toggle_max = 4; // Set the maximum number of states

byte leds = 0;
byte red_led = 1;
byte yellow_led = 2;
byte green_led = 4;

Oscillator oscillator1 = Oscillator(2); // y1 = sin(2*pi*(1/2)*t)
Oscillator oscillator2 = Oscillator(2, 2); // y2 = sin(2*pi*(1/2)t + 2*(pi/12))

bool first_loop = true;

// Function for implementation of Shift register 74HC595
void update_shift_register()
{
   digitalWrite(latch_pin, LOW);
   shiftOut(data_pin, clock_pin, LSBFIRST, leds);
   digitalWrite(latch_pin, HIGH);
}

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
  
  // Pins for Shift Register
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  if (first_loop) {
    update_shift_register();
    first_loop = false;
  }

  // Handle button press
  if (digitalRead(BUTTON_PIN) == 0) {
    button_toggle += 1;
    button_toggle = button_toggle % toggle_max; // Ensure button_toggle < toggle_max
    Serial.println(button_toggle);
    
    // Update shift register
    if (button_toggle == 1) { leds = red_led; }
    else if (button_toggle == 2) { leds = yellow_led; }
    else if (button_toggle == 3) { leds = green_led; }
    else { leds = 0; }
    update_shift_register();
    
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
  if (button_toggle == 1) {
    if (A_val == 0) {
      // Clockwise -> increment
      if (B_val == 1) { oscillator1.increment_period(); }
      else { oscillator1.decrement_period(); }

      delay(100);
    }
  }
  else if (button_toggle == 2) {
    if (A_val == 0) {
      // Clockwise -> increment
      if (B_val == 1) { oscillator2.increment_period(); }
      else { oscillator2.decrement_period(); }
      
      delay(100);
    }
  }
  else if (button_toggle == 3) {
    if (A_val == 0) {
     if (B_val == 1) { oscillator2.increment_phi(); }
     else { oscillator2.decrement_phi(); }
      
     oscillator1.reset_param();
     oscillator2.reset_param();
     delay(100);
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

  // 
  oscillator1.update();
  oscillator2.update();
  delay(Oscillator::delay_time);
}
