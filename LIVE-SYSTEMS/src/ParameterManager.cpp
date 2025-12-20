#include "../include/ParameterManager.h"
#include <juce_core/juce_core.h>

//==============================================================================
ParameterManager::ParameterManager(juce::AudioProcessor& processor)
    : processorRef(processor),
      apvts(processor, nullptr, "Parameters", {})
{
}

ParameterManager::~ParameterManager()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ParameterManager::createAPVTSLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Add owned parameters to the layout
    for (auto& param : ownedParameters)
    {
        layout.add(std::move(param));
    }
    
    return layout;
}

//==============================================================================
juce::AudioParameterFloat* ParameterManager::addFloatParameter(const ParameterInfo& info)
{
    auto* param = new juce::AudioParameterFloat(
        juce::ParameterID(info.id, 1),  // ParameterID with version
        info.name,
        juce::NormalisableRange<float>(info.minValue, info.maxValue),
        info.defaultValue,
        info.units,
        juce::AudioProcessorParameter::genericParameter,
        [info](float value, int) { return info.valueToText ? info.valueToText(value) : juce::String(value); },
        [info](const juce::String& text) { return info.textToValue ? info.textToValue(text) : text.getFloatValue(); }
    );
    
    parameters[info.id] = param;
    apvts.createAndAddParameter(std::unique_ptr<juce::AudioParameterFloat>(param));
    
    return param;
}

juce::AudioParameterChoice* ParameterManager::addChoiceParameter(const juce::String& id,
                                                                 const juce::String& name,
                                                                 const juce::StringArray& choices,
                                                                 int defaultIndex)
{
    auto* param = new juce::AudioParameterChoice(
        juce::ParameterID(id, 1), 
        name, 
        choices, 
        defaultIndex
    );
    
    parameters[id] = param;
    apvts.createAndAddParameter(std::unique_ptr<juce::AudioParameterChoice>(param));
    
    return param;
}

juce::AudioParameterBool* ParameterManager::addBoolParameter(const juce::String& id,
                                                             const juce::String& name,
                                                             bool defaultValue)
{
    auto* param = new juce::AudioParameterBool(
        juce::ParameterID(id, 1), 
        name, 
        defaultValue
    );
    
    parameters[id] = param;
    apvts.createAndAddParameter(std::unique_ptr<juce::AudioParameterBool>(param));
    
    return param;
}

//==============================================================================
juce::RangedAudioParameter* ParameterManager::getParameter(const juce::String& id)
{
    auto it = parameters.find(id);
    return (it != parameters.end()) ? it->second : nullptr;
}

float ParameterManager::getParameterValue(const juce::String& id)
{
    if (auto* raw = apvts.getRawParameterValue(id))
        return raw->load();
    return 0.0f;
}

void ParameterManager::setParameterValue(const juce::String& id, float value)
{
    if (auto* parameter = apvts.getParameter(id))
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

//==============================================================================
void ParameterManager::createParameterGroup(const juce::String& groupName, const juce::StringArray& parameterIds)
{
    parameterGroups[groupName] = parameterIds;
}

juce::StringArray ParameterManager::getParameterGroup(const juce::String& groupName)
{
    auto it = parameterGroups.find(groupName);
    return (it != parameterGroups.end()) ? it->second : juce::StringArray();
}

//==============================================================================
void ParameterManager::addParameterListener(const juce::String& id, std::function<void(float)> callback)
{
    parameterListeners[id] = callback;
}

void ParameterManager::removeParameterListener(const juce::String& id)
{
    parameterListeners.erase(id);
}

//==============================================================================
juce::AudioProcessorParameterGroup ParameterManager::createParameterLayout()
{
    juce::AudioProcessorParameterGroup layout;
    
    // Add all owned parameters to the layout
    for (auto& param : ownedParameters)
    {
        layout.addChild(std::move(param));
    }
    
    return layout;
}

//==============================================================================
void ParameterManager::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void ParameterManager::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
            apvts.replaceState(state);
    }
}