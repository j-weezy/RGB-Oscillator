/*
    Oscillator.h
    By Jason Wirth
    Last updated 07/04/2024

    Declaration of class Oscillator for use with oscillation Arduino project.
    Can be interacted with by changing period and phase constant.

    y(t) = sin(2*pi*f*t + h)
*/
class Oscillator {
    private:
        float param; // t
        float period; // T = 1/f
        float freq; // f
        static const float phi_base;
        static const int phi_factor_max;
        int phi_factor;
        float phi; // phi = phi_base * phi_factor
        float oscillator_state;
        static const float period_min;
        static const float period_max;
        static const float period_margin;
        static const float period_marginal_increment;
        static const float period_increment;
        static const float step;

        void update_freq();
        void update_phi();
    
    public:
        Oscillator(float initial_period, float initial_phi=0);
        static const int delay_time; // Exposed for access by Arduino loop()
        void reset_param();
        void increment_period();
        void decrement_period();
        void increment_phi();
        void decrement_phi();
        float get_period();
        int get_phi(); // Returns phi_factor, indicating an integer multiple of phi_base = pi / 12
        float state();
        void update();
};