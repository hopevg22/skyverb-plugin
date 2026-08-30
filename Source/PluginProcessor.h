#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "RoomReverb.h"

class BigSkyCloneAudioProcessor : public juce::AudioProcessor
{
public:
    BigSkyCloneAudioProcessor();
    ~BigSkyCloneAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SkyVerb (Room)"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Footswitch preset slots (A=0, B=1, C=2). Snapshot the current knob
    // state into a slot, or recall a previously-saved slot. Slots persist
    // across save/reload via getStateInformation/setStateInformation.
    static constexpr int kNumPresetSlots = 3;
    void savePreset (int slot);
    void loadPreset (int slot);
    bool isPresetSlotFilled (int slot) const;

    // Parameter IDs (also used to wire up the editor's knobs)
    static constexpr auto idDecay      = "decay";
    static constexpr auto idPreDelay   = "predelay";
    static constexpr auto idTone       = "tone";
    static constexpr auto idMix        = "mix";
    static constexpr auto idParam1Size = "param1_size";
    static constexpr auto idParam2Diff = "param2_diffusion";
    static constexpr auto idMod        = "mod";

private:
    RoomReverb reverb;
    std::array<juce::ValueTree, kNumPresetSlots> presetSlots;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BigSkyCloneAudioProcessor)
};
