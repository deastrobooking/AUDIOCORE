#include "../include/ParameterManager.h"
#include <juce_core/juce_core.h>

//==============================================================================
ParameterManager::ParameterManager()
{
}

ParameterManager::~ParameterManager()
{
}

//==============================================================================
juce::AudioParameterFloat* ParameterManager::addFloatParameter(const ParameterInfo& info)
{
    auto param = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(info.id, 1),  // ParameterID with version
        info.name,
        juce::NormalisableRange<float>(info.minValue, info.maxValue),
        info.defaultValue,
        info.units,
        juce::AudioProcessorParameter::genericParameter,
        [info](float value, int) { return info.valueToText ? info.valueToText(value) : juce::String(value); },
        [info](const juce::String& text) { return info.textToValue ? info.textToValue(text) : text.getFloatValue(); }
    );
    
    auto* paramPtr = param.get();
    parameters[info.id] = paramPtr;
    ownedParameters.push_back(std::move(param));
    
    return paramPtr;
}

juce::AudioParameterChoice* ParameterManager::addChoiceParameter(const juce::String& id,
                                                                 const juce::String& name,
                                                                 const juce::StringArray& choices,
                                                                 int defaultIndex)
{
    auto param = std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(id, 1), 
        name, 
        choices, 
        defaultIndex
    );
    
    auto* paramPtr = param.get();
    parameters[id] = paramPtr;
    ownedParameters.push_back(std::move(param));
    
    return paramPtr;
}

juce::AudioParameterBool* ParameterManager::addBoolParameter(const juce::String& id,
                                                             const juce::String& name,
                                                             bool defaultValue)
{
    auto param = std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(id, 1), 
        name, 
        defaultValue
    );
    
    auto* paramPtr = param.get();
    parameters[id] = paramPtr;
    ownedParameters.push_back(std::move(param));
    
    return paramPtr;
}

//==============================================================================
juce::RangedAudioParameter* ParameterManager::getParameter(const juce::String& id)
{
    auto it = parameters.find(id);
    return (it != parameters.end()) ? it->second : nullptr;
}

float ParameterManager::getParameterValue(const juce::String& id)
{
    if (auto* param = getParameter(id))
        return param->getValue();
    return 0.0f;
}

void ParameterManager::setParameterValue(const juce::String& id, float value)
{
    if (auto* param = getParameter(id))
        param->setValue(value);
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
    juce::ValueTree state("Parameters");
    
    for (const auto& [id, param] : parameters)
    {
        state.setProperty(id, param->getValue(), nullptr);
    }
    
    auto xml = state.createXml();
    if (xml != nullptr)
        juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void ParameterManager::setStateInformation(const void* data, int sizeInBytes)
{
    auto xml = juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes);
    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
        {
            for (const auto& [id, param] : parameters)
            {
                if (state.hasProperty(id))
                    param->setValue(static_cast<float>(state.getProperty(id)));
            }
        }
    }
}