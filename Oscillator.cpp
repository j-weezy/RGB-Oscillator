/*
    Oscillator.cpp
    
    Definition of Oscillator class for use with oscillation Arduino project.
*/

#include "Oscillator.h"
#include <math.h>

const float pi = 3.1415;
const float tau = 2 * pi;

const float Oscillator::period_min = 0.125;
const float Oscillator::period_max = 2;
const float Oscillator::period_margin = 0.5;
const float Oscillator::period_marginal_increment = 0.05;
const float Oscillator::period_increment = 0.1;
const float Oscillator::phi_increment = pi / 12;
const float Oscillator::step = 0.01;

const int Oscillator::delay_time = 10;

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

void Oscillator::reset_param() {
    param = 0;
}

void Oscillator::increment_period() {
    if (period < period_max) {
        if (period > period_margin) { period += period_marginal_increment; }
        else { period += period_increment; }
        update_freq();
    }
}

void Oscillator::decrement_period() {
    if (period > period_min) {
        if (period > period_margin) { period -= period_marginal_increment; }
        else { period -= period_increment; }
        update_freq();
    }
}

void Oscillator::increment_phi() {
    if (phi < tau) { phi += phi_increment; }
}

void Oscillator::decrement_phi() {
    if (phi > 0) { phi -= phi_increment; }
}

float Oscillator::state() {
    return oscillator_state;
}

void Oscillator::update() {
    oscillator_state = sin(tau * freq * param + phi);
    param += step;
    if (param >= period) { param = 0; }
}