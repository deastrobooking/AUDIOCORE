#include "SimpleDistortionProcessor.h"
#include "SimpleDistortionEditor.h"
#include <cmath>

//==============================================================================
SimpleDistortionProcessor::SimpleDistortionProcessor()
{
    // Initialize parameters after base class construction is complete
    initializeParameters();
}

SimpleDistortionProcessor::~SimpleDistortionProcessor()
{
}

//==============================================================================
void SimpleDistortionProcessor::initializeParameters()
{
    // Drive parameter (1.0 to 10.0)
    ParameterManager::ParameterInfo driveInfo;
    driveInfo.id = DRIVE_PARAM_ID;
    driveInfo.name = "Drive";
    driveInfo.defaultValue = 2.0f;
    driveInfo.minValue = 1.0f;
    driveInfo.maxValue = 10.0f;
    driveInfo.units = "";
    driveInfo.valueToText = [](float value) { return juce::String(value, 1) + "x"; };
    driveInfo.textToValue = [](const juce::String& text) { return text.getFloatValue(); };
    
    parameterManager->addFloatParameter(driveInfo);

    // Output level parameter (-20.0 to 20.0 dB)
    ParameterManager::ParameterInfo outputInfo;
    outputInfo.id = OUTPUT_PARAM_ID;
    outputInfo.name = "Output";
    outputInfo.defaultValue = 0.0f;
    outputInfo.minValue = -20.0f;
    outputInfo.maxValue = 20.0f;
    outputInfo.units = "dB";
    outputInfo.valueToText = [](float value) { return juce::String(value, 1) + " dB"; };
    outputInfo.textToValue = [](const juce::String& text) { return text.getFloatValue(); };
    
    parameterManager->addFloatParameter(outputInfo);

    // Mix parameter (0.0 to 1.0)
    ParameterManager::ParameterInfo mixInfo;
    mixInfo.id = MIX_PARAM_ID;
    mixInfo.name = "Mix";
    mixInfo.defaultValue = 1.0f;
    mixInfo.minValue = 0.0f;
    mixInfo.maxValue = 1.0f;
    mixInfo.units = "%";
    mixInfo.valueToText = [](float value) { return juce::String(static_cast<int>(value * 100)) + "%"; };
    mixInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    
    parameterManager->addFloatParameter(mixInfo);

    // Create parameter groups for organization
    parameterManager->createParameterGroup("Distortion", { DRIVE_PARAM_ID, MIX_PARAM_ID });
    parameterManager->createParameterGroup("Output", { OUTPUT_PARAM_ID });
}

void SimpleDistortionProcessor::processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    // Get parameter values
    const float drive = parameterManager->getParameterValue(DRIVE_PARAM_ID);
    const float outputGain = juce::Decibels::decibelsToGain(parameterManager->getParameterValue(OUTPUT_PARAM_ID));
    const float mix = parameterManager->getParameterValue(MIX_PARAM_ID);

    // Process each channel
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float inputSample = channelData[sample];
            const float distortedSample = applyDistortion(inputSample, drive);
            
            // Apply mix and output gain
            channelData[sample] = (inputSample * (1.0f - mix) + distortedSample * mix) * outputGain;
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* SimpleDistortionProcessor::createEditor()
{
    return new SimpleDistortionEditor(*this);
}

//==============================================================================
float SimpleDistortionProcessor::applyDistortion(float sample, float drive)
{
    // Simple waveshaping distortion
    const float driven = sample * drive;
    
    // Soft clipping using tanh
    return std::tanh(driven) / drive;
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDistortionProcessor();
}