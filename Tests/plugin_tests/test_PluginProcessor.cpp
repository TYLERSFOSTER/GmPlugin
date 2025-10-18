#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../../Source/plugin/PluginProcessor.h"

using Catch::Approx;  // ✅ fixes "Approx undeclared" errors

// ============================================================
// 1. Basic sanity: processor produces non-silent output
// ============================================================
TEST_CASE("GmPluginAudioProcessor basic processing", "[plugin]")
{
    GmPluginAudioProcessor proc;
    proc.prepareToPlay(48000.0, 512);

    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midi;
    buffer.clear();

    proc.processBlock(buffer, midi);

    bool anyNonZero = false;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            if (std::abs(buffer.getSample(ch, i)) > 1.0e-6f)
                anyNonZero = true;

    REQUIRE(anyNonZero);

    proc.releaseResources();
}

// ============================================================
// 2a. MIDI CC mapping via public accessor
// ============================================================
TEST_CASE("PluginProcessor: MIDI CC mapping updates knobState (accessor)", "[plugin]")
{
    GmPluginAudioProcessor proc;
    juce::AudioBuffer<float> buffer(2, 64);
    juce::MidiBuffer midi;

    // Simulate CC1 (Amplitude)
    midi.addEvent(juce::MidiMessage::controllerEvent(1, 1, 127), 0);
    proc.processBlock(buffer, midi);
    REQUIRE(proc.getKnobState().A_dB == Approx(24.0f).margin(0.1f));

    // Simulate CC2 (Frequency)
    midi.clear();
    midi.addEvent(juce::MidiMessage::controllerEvent(1, 2, 64), 0);
    proc.processBlock(buffer, midi);
    REQUIRE(proc.getKnobState().lambda_hz > 20.0f);
    REQUIRE(proc.getKnobState().lambda_hz < 20000.0f);
}

// ============================================================
// 2b. MIDI CC mapping direct-value test (alternate coverage)
// ============================================================
TEST_CASE("PluginProcessor: MIDI CC mapping updates knobState (value checks)", "[plugin]")
{
    GmPluginAudioProcessor proc;
    juce::AudioBuffer<float> buffer(1, 32);
    juce::MidiBuffer midi;

    // CC1 → amplitude
    midi.addEvent(juce::MidiMessage::controllerEvent(1, 1, 127), 0);
    proc.processBlock(buffer, midi);
    REQUIRE(proc.getKnobState().A_dB == Approx(24.0f));

    // CC2 → frequency
    midi.clear();
    midi.addEvent(juce::MidiMessage::controllerEvent(1, 2, 127), 0);
    proc.processBlock(buffer, midi);
    REQUIRE(proc.getKnobState().lambda_hz > 10000.0f);

    // CC3 → phase offset
    midi.clear();
    midi.addEvent(juce::MidiMessage::controllerEvent(1, 3, 127), 0);
    proc.processBlock(buffer, midi);
    REQUIRE(proc.getKnobState().phase_offset_rad == Approx(juce::MathConstants<float>::twoPi));
}
