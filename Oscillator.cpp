/*
    Oscillator.cpp
    
    Definition of Oscillator class for use with oscillation Arduino project.
*/

#include "Oscillator.h"
#include <math.h>

const float pi = 3.1415;
const float tau = 2 * pi;

const float Oscillator::period_min = 0.125; // At this point, the oscillation is no longer discernable
const float Oscillator::period_max = 4; // If the oscillation is too slow, 
const float Oscillator::period_margin = 0.5;
const float Oscillator::period_marginal_increment = 0.05;
const float Oscillator::period_increment = 0.1;
const float Oscillator::phi_base = pi / 12;

const float Oscillator::step = 0.01;
const int Oscillator::delay_time = 10; // Each step takes this many milliseconds

Oscillator::Oscillator(float initial_period, float initial_phi=0) {
    period = initial_period;
    phi = initial_phi * pi / 12;
    param = 0;
    update_freq();
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