// ===================== Source/dsp/Oscillator.h =====================
#pragma once
#include <cmath>

struct Oscillator {
    float sampleRate = 48000.0f;   // default; set once from host
    float phase = 0.0f;            // internal running phase [radians]
    static constexpr float TWO_PI = 6.283185307f;

    void prepare(float sr) noexcept {
        sampleRate = sr;
        phase = 0.0f;
    }

    /// Simple sine oscillator driven by explicit parameters.
    inline float process(float lambda_hz, float phase_offset_rad, float A_dB) noexcept {
        // Frequency increment per sample
        const float dphi = TWO_PI * lambda_hz / sampleRate;

        // Amplitude from dB
        constexpr float LN10_OVER_20 = 0.115129254f;
        const float amp = std::exp(A_dB * LN10_OVER_20);

        // Total phase including offset
        const float phi_total = phase + phase_offset_rad;

        // Signal
        const float out = amp * std::cos(phi_total);

        // advance and wrap
        phase += dphi;
        if (phase >= TWO_PI)
            phase -= TWO_PI;

        return out;
    }
};
