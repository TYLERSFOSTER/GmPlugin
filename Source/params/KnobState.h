// ===================== KnobState.h =====================
#pragma once
#include "ParameterIDs.h"
#include "ParamSpec.h"
#include <atomic>
#include <cmath>

struct KnobState {
    // Core knob values (all in the most direct, DSP-friendly units)
    std::atomic<float> u_mag_dB         { 0.0f };
    std::atomic<float> u_theta_rad      { 0.0f };
    std::atomic<float> lambda_hz        { 440.0f };
    std::atomic<float> A_dB             { 0.0f };
    std::atomic<float> clip_enable      { 1.0f };
    std::atomic<float> phase_offset_rad { 0.0f };

    // ------------------------------------------------------------------
    // Convenience getters (computed forms used in DSP)
    // ------------------------------------------------------------------

    /// A small, fast, side-effect-free, non-throwing function that computes a float value.
    inline float totalAmpLinear() const noexcept {
        const auto& A = PARAM_SPECS[(int)ParameterID::A_dB];
        const auto& u = PARAM_SPECS[(int)ParameterID::u_mag_dB];

        // Apply per-parameter linearization using the helper in ParamSpec.h
        return toLinear(A, A_dB.load()) * toLinear(u, u_mag_dB.load());
    }

    /// A small, fast, side-effect-free, non-throwing function that computes a float value.
    inline float uPhase() const noexcept {
        return u_theta_rad.load() + phase_offset_rad.load();
    }

    /// A small, fast, side-effect-free, non-throwing function that computes a float value.
    inline bool clippingEnabled() const noexcept {
        return clip_enable.load() >= 0.5f;
    }
};
