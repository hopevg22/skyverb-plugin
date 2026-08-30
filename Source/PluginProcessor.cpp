#include "PluginProcessor.h"
#include "PluginEditor.h"

BigSkyCloneAudioProcessor::BigSkyCloneAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BigSkyCloneAudioProcessor::createParameterLayout()
{
    using Range = juce::NormalisableRange<float>;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idDecay, "Decay", Range (0.0f, 1.0f, 0.001f), 0.4f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idPreDelay, "Pre-Delay", Range (0.0f, 250.0f, 0.1f), 20.0f, "ms"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idTone, "Tone", Range (0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idMix, "Mix", Range (0.0f, 1.0f, 0.001f), 0.35f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idParam1Size, "Param 1 (Size)", Range (0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idParam2Diff, "Param 2 (Diffusion)", Range (0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        idMod, "Mod", Range (0.0f, 1.0f, 0.001f), 0.2f));

    return { params.begin(), params.end() };
}

void BigSkyCloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverb.prepare (sampleRate, samplesPerBlock);
}

bool BigSkyCloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto set = layouts.getMainOutputChannelSet();
    return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void BigSkyCloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    // If host only gives us mono, duplicate to a temp stereo buffer path is
    // unnecessary here since RoomReverb::processBlock reads channel 0/1 and
    // guards for channel count internally.
    reverb.setParameters (
        apvts.getRawParameterValue (idDecay)->load(),
        apvts.getRawParameterValue (idPreDelay)->load(),
        apvts.getRawParameterValue (idTone)->load(),
        apvts.getRawParameterValue (idParam1Size)->load(),
        apvts.getRawParameterValue (idParam2Diff)->load(),
        apvts.getRawParameterValue (idMod)->load(),
        apvts.getRawParameterValue (idMix)->load());

    reverb.processBlock (buffer);
}

void BigSkyCloneAudioProcessor::savePreset (int slot)
{
    if (slot < 0 || slot >= kNumPresetSlots) return;
    presetSlots[(size_t) slot] = apvts.copyState();
}

void BigSkyCloneAudioProcessor::loadPreset (int slot)
{
    if (slot < 0 || slot >= kNumPresetSlots) return;
    if (presetSlots[(size_t) slot].isValid())
        apvts.replaceState (presetSlots[(size_t) slot].createCopy());
}

bool BigSkyCloneAudioProcessor::isPresetSlotFilled (int slot) const
{
    if (slot < 0 || slot >= kNumPresetSlots) return false;
    return presetSlots[(size_t) slot].isValid();
}

void BigSkyCloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree root ("BigSkyCloneState");
    root.appendChild (apvts.copyState(), nullptr);

    for (int i = 0; i < kNumPresetSlots; ++i)
    {
        if (presetSlots[(size_t) i].isValid())
        {
            auto slotCopy = presetSlots[(size_t) i].createCopy();
            slotCopy.setProperty ("presetSlotIndex", i, nullptr);
            root.appendChild (slotCopy, nullptr);
        }
    }

    if (auto xml = root.createXml())
        copyXmlToBinary (*xml, destData);
}

void BigSkyCloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary (data, sizeInBytes);
    if (xml == nullptr) return;

    auto root = juce::ValueTree::fromXml (*xml);
    if (! root.isValid()) return;

    for (int i = 0; i < kNumPresetSlots; ++i)
        presetSlots[(size_t) i] = juce::ValueTree();

    for (int i = 0; i < root.getNumChildren(); ++i)
    {
        auto child = root.getChild (i);
        if (child.hasProperty ("presetSlotIndex"))
        {
            int slot = (int) child.getProperty ("presetSlotIndex");
            auto clean = child.createCopy();
            clean.removeProperty ("presetSlotIndex", nullptr);
            if (slot >= 0 && slot < kNumPresetSlots)
                presetSlots[(size_t) slot] = clean;
        }
        else
        {
            apvts.replaceState (child);
        }
    }
}

juce::AudioProcessorEditor* BigSkyCloneAudioProcessor::createEditor()
{
    return new BigSkyCloneAudioProcessorEditor (*this);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BigSkyCloneAudioProcessor();
}
