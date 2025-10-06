#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <unordered_map>

//==============================================================================
/**
    Modern parameter management system for LIVE-SYSTEMS plugins.
    Provides automatic parameter creation, value mapping, and automation support.
*/
class ParameterManager
{
public:
    //==============================================================================
    struct ParameterInfo
    {
        juce::String id;
        juce::String name;
        float defaultValue;
        float minValue;
        float maxValue;
        juce::String units;
        std::function<juce::String(float)> valueToText;
        std::function<float(const juce::String&)> textToValue;
    };

    //==============================================================================
    ParameterManager();
    ~ParameterManager();

    //==============================================================================
    // Parameter creation and management
    juce::AudioParameterFloat* addFloatParameter(const ParameterInfo& info);
    juce::AudioParameterChoice* addChoiceParameter(const juce::String& id,
                                                   const juce::String& name,
                                                   const juce::StringArray& choices,
                                                   int defaultIndex);
    juce::AudioParameterBool* addBoolParameter(const juce::String& id,
                                               const juce::String& name,
                                               bool defaultValue);

    //==============================================================================
    // Parameter access
    juce::RangedAudioParameter* getParameter(const juce::String& id);
    float getParameterValue(const juce::String& id);
    void setParameterValue(const juce::String& id, float value);

    //==============================================================================
    // Parameter groups and layout
    void createParameterGroup(const juce::String& groupName, const juce::StringArray& parameterIds);
    juce::StringArray getParameterGroup(const juce::String& groupName);

    //==============================================================================
    // Automation and modulation
    void addParameterListener(const juce::String& id, std::function<void(float)> callback);
    void removeParameterListener(const juce::String& id);

    //==============================================================================
    // State management
    void getStateInformation(juce::MemoryBlock& destData);
    void setStateInformation(const void* data, int sizeInBytes);

    //==============================================================================
    // Get all parameters for processor registration
    juce::AudioProcessorParameterGroup createParameterLayout();

private:
    std::unordered_map<juce::String, juce::RangedAudioParameter*> parameters;
    std::unordered_map<juce::String, juce::StringArray> parameterGroups;
    std::unordered_map<juce::String, std::function<void(float)>> parameterListeners;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> ownedParameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterManager)
};