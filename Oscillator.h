/*
    Oscillator.h

    Declaration of class Oscillator for use with oscillation Arduino project.
    Can be interacted with by changing period and phase constant.

    y(t) = sin(2*pi*f*t + h)
*/
class Oscillator {
    private:
        float param; // t
        float period; // T = 1/f
        float freq; // f
        float phi; // h
        float oscillator_state;
        static const float period_min; 
        static const float period_max;
        static const float period_margin;
        static const float period_marginal_increment;
        static const float period_increment;
        static const float phi_increment;
        static const float step;

        void update_freq();
    
    public:
        Oscillator(float initial_period, float initial_phi=0);
        static const int delay_time; // Exposed for access by Arduino loop()
        void reset_param();
        void increment_period();
        void decrement_period();
        void increment_phi();
        void decrement_phi();
        float state();
        void update() ;
};