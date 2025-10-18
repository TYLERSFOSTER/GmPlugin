#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <unordered_map>
#include "../dsp/Oscillator.h"
#include "../params/KnobState.h"

class GmPluginAudioProcessor : public juce::AudioProcessor
{
public:
    // ============================================================
    // Constructor / Destructor
    // ============================================================
    GmPluginAudioProcessor();
    ~GmPluginAudioProcessor() override;

    // ============================================================
    // AudioProcessor overrides
    // ============================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // ============================================================
    // Required program management methods
    // ============================================================
    int getNumPrograms() override                { return 1; }
    int getCurrentProgram() override             { return 0; }
    void setCurrentProgram(int) override         {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    // ============================================================
    // Basic metadata
    // ============================================================
    const juce::String getName() const override  { return "GmPlugin"; }

    bool hasEditor() const override              { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }

    bool acceptsMidi() const override            { return true; }
    bool producesMidi() const override           { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    // ============================================================
    // Testing accessors (read-only)
    // ============================================================
    const KnobState& getKnobState() const noexcept { return knobState; }

private:
    // -------------------- Polyphony --------------------
    struct Voice {
        Oscillator osc;  // per-note oscillator (phase per voice)
        float ratio = 1.0f; // 2^((note-69)/12) relative to global lambda_hz
    };

    std::unordered_map<int, Voice> voices; // note -> voice

    // -------------------- Global state --------------------
    KnobState knobState;       // A_dB, lambda_hz, phase_offset_rad (global)
    float sampleRateHz = 48000.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GmPluginAudioProcessor)
};
