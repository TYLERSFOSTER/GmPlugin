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

    /// Sine oscillator driven by explicit parameters.
    /// lambda_hz: base frequency (per-voice)
    /// phase_offset_rad: global phase (host/UI)
    /// A_dB: base amplitude in dB (host/UI)
    /// u_r_mag: extra linear gain multiplier (complex radius r)
    /// u_phi_rad: extra phase rotation in radians (complex angle φ)
    inline float process(float lambda_hz,
                         float phase_offset_rad,
                         float A_dB,
                         float u_r_mag,
                         float u_phi_rad) noexcept
    {
        // Frequency increment per sample
        const float dphi = TWO_PI * lambda_hz / sampleRate;

        // Amplitude from dB, then apply complex radius r
        constexpr float LN10_OVER_20 = 0.115129254f;
        const float amp = std::exp(A_dB * LN10_OVER_20) * u_r_mag;

        // Total phase including offsets and complex φ
        const float phi_total = phase + phase_offset_rad + u_phi_rad;

        // Signal
        const float out = amp * std::cos(phi_total);

        // advance and wrap
        phase += dphi;
        if (phase >= TWO_PI)
            phase -= TWO_PI;

        return out;
    }
};
