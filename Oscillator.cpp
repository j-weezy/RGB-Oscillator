/*
    Oscillator.cpp
    
    Definition of Oscillator class for use with oscillation Arduino project.

    Object for simulating Simple Harmonic Motion, i.e. a sine wave.
    The period (T) of the oscillation is defined by the user and the object calculates the frequency (f).
    The oscillation is modeled as
        y(t) = sin(2*pi*f*t + h)
    Where h is a phase constant for use with two or more oscillators.
    When the phase constant is used, all param values should be synchronized via reset_param().

    The timing is handled by the step and delay_time constants.
    Each update will increment param. Call delay(delay_time) in an otherwise uninterrupted loop.
    Setting step = delay_time / 1000 will result in seconds (s) as the units of param.
    Note that delay_time is offered as an integer value to be passed to delay() rather than directly calling delay() with a class method, to avoid including Arduino.h in this library.
*/

#include "Oscillator.h"
#include <math.h>

const float pi = 3.1415; // This is probably more precise than necessary
const float tau = 2 * pi;

const float Oscillator::period_min = 0.13; // At this point, the oscillation is no longer discernable
const float Oscillator::period_max = 4; // The oscillation needs to remain fast enough to appear continuous
const float Oscillator::period_margin = 0.5; // For small period values, changes in the period result in exaggerated changes in frequency
const float Oscillator::period_marginal_increment = 0.05;
const float Oscillator::period_increment = 0.1;
const float Oscillator::phi_base = pi / 12; // pi / 12 is chosen because 12 is divisible by all the common radian value denominators (2, 3, 4)
const int Oscillator::phi_factor_max = 24; // Range of phi should be [0, 2*pi]

const float Oscillator::step = 0.01;
const int Oscillator::delay_time = 10; // Each step takes this many milliseconds

Oscillator::Oscillator(float initial_period, float initial_phi=0) {
    if ((period_min < period) && (period < period_max)) { 
        period = initial_period;
    }
    else {
        period = 2;
    }
    if ((-1 < initial_phi) && (initial_phi < phi_factor_max)) {
        phi_factor = initial_phi;
    }
    else {
        phi_factor = 0;
    }
    param = 0; // Initialize param (t)
    update_freq();
    update_phi();
    update();
}

void Oscillator::update_freq() {
    // Frequency (in Hz) is the reciprocal of the period (in s)
    freq = 1 / period;
}

void Oscillator::update_phi() {
    // Phi will be restricted to multiples of phi_base = pi / 12
    phi = phi_base * phi_factor;
}

void Oscillator::reset_param() {
    param = 0;
}

void Oscillator::increment_period() {
    if (period < period_max) {
        if (period < period_margin) { period += period_marginal_increment; }
        else { period += period_increment; }
        update_freq();
    }
}

void Oscillator::decrement_period() {
    if (period > period_min) {
        if (period < period_margin) { period -= period_marginal_increment; }
        else { period -= period_increment; }
        update_freq();
    }
}

void Oscillator::increment_phi() {
    if (phi_factor < 24) { phi_factor += 1; }
    update_phi();
}

void Oscillator::decrement_phi() {
    if (phi_factor > 0) { phi_factor -= 1; }
    update_phi();
}

float Oscillator::state() {
    return oscillator_state;
}

float Oscillator::get_period() {
    if (period) { return period; }
    else { return 0; }
}

int Oscillator::get_phi() {
    if (phi_factor) { return phi_factor; }
    else { return 0; }
}

void Oscillator::update() {
    oscillator_state = sin(tau * freq * param + phi);
    param += step;
    if (param >= period) { param = 0; }
}