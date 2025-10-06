#pragma once

#include "../../include/PluginProcessor.h"

//==============================================================================
/**
    Simple distortion plugin example that demonstrates how to use the LIVE-SYSTEMS framework.
    This plugin applies basic waveshaping distortion with drive and output level controls.
*/
class SimpleDistortionProcessor : public LiveSystemsProcessor
{
public:
    //==============================================================================
    SimpleDistortionProcessor();
    ~SimpleDistortionProcessor() override;

    //==============================================================================
    // Framework overrides
    void initializeParameters() override;
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::String getPluginName() const override { return "Simple Distortion"; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

private:
    //==============================================================================
    // Parameter IDs
    static constexpr const char* DRIVE_PARAM_ID = "drive";
    static constexpr const char* OUTPUT_PARAM_ID = "output";
    static constexpr const char* MIX_PARAM_ID = "mix";

    // DSP
    float applyDistortion(float sample, float drive);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDistortionProcessor)
};