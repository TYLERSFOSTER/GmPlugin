// ===================== Source/dsp/Oscillator.h =====================
#pragma once
#include <cmath>

struct Oscillator {
    float sampleRate = 48000.0f;   // set once from host
    float phase      = 0.0f;       // running phase [radians]

    static constexpr float PI     = 3.14159265358979323846f;
    static constexpr float TWO_PI = 6.28318530717958647693f;
    static constexpr float LN10_OVER_20 = 0.115129254f; // ln(10)/20

    void prepare(float sr) noexcept {
        sampleRate = sr;
        phase = 0.0f;
    }

    // Wrap angle to (-pi, pi]
    static inline float wrapToPi(float x) noexcept {
        x = std::fmod(x + PI, TWO_PI);
        if (x < 0.0f) x += TWO_PI;
        return x - PI;
    }

    // Literal complex-exponent modulation:
    // out = Re{ exp( (ln A + i*theta) * (r * e^{i*phi}) ) }
    inline float process(float lambda_hz,
                         float phase_offset_rad,
                         float A_dB,
                         float u_r_mag,
                         float u_phi_rad) noexcept
    {
        // Base phase increment per sample
        const float dtheta = TWO_PI * lambda_hz / sampleRate;

        // ln(A) from dB:  ln(A_lin) = A_dB * ln(10)/20
        const float L = A_dB * LN10_OVER_20;

        // Current base angle theta (wrapped to keep things bounded)
        const float theta = pow(wrapToPi(phase + phase_offset_rad), 2);

        // u = r * e^{i phi}
        const float c = std::cos(u_phi_rad);
        const float s = std::sin(u_phi_rad);

        // Real/Imag parts of (ln A + i theta) * u
        const float realPart = u_r_mag * ( L * c - theta * s );
        const float imagPart = u_r_mag * ( L * s + theta * c );

        // Output: magnitude = exp(realPart); phase = imagPart
        const float out = std::exp(realPart) * std::cos(imagPart);

        // Advance unwrapped phase (host pitch)
        phase += dtheta;
        if (phase >= TWO_PI) phase -= TWO_PI;

        return out;
    }
};
