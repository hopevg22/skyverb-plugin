#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "BigSkyLookAndFeel.h"

class BigSkyCloneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BigSkyCloneAudioProcessorEditor (BigSkyCloneAudioProcessor&);
    ~BigSkyCloneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct KnobWithLabel
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    void setupKnob (KnobWithLabel& k, const juce::String& text, const juce::String& paramId);
    void footswitchClicked (int slot, char letter);
    void drawAlgorithmRing (juce::Graphics& g); // the two-column LED/label selector around the encoder

    BigSkyCloneAudioProcessor& processor;
    BigSkyLookAndFeel lookAndFeel;

    // encoder + algorithm-type indicator (only "Room" is implemented so far;
    // the rest of the ring is drawn as a preview of future algorithms)
    juce::Slider valueEncoder { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label valueLabel, typeLabel, algoNameLabel;

    KnobWithLabel toneKnob, decayKnob, preDelayKnob, mixKnob;
    KnobWithLabel param1Knob, param2Knob, modKnob;

    juce::TextButton footswitchA { "A" }, footswitchB { "B" }, footswitchC { "C" };
    juce::Label brandLabel, modelLabel, lcdLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BigSkyCloneAudioProcessorEditor)
};
