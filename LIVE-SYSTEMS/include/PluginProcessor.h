#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterManager.h"
#include "AudioEngine.h"
#include "PresetManager.h"

//==============================================================================
/**
    Base class for LIVE-SYSTEMS audio plugins.
    Provides a modern framework for VST plugin development with parameter management,
    preset handling, and audio processing capabilities.
*/
class LiveSystemsProcessor : public juce::AudioProcessor
{
public:
    enum class EffectChainPosition
    {
        PreSource,
        PostSource
    };

    //==============================================================================
    LiveSystemsProcessor();
    ~LiveSystemsProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Framework methods for derived classes to override
    virtual void initializeParameters() = 0;
    virtual void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) = 0;
    virtual juce::String getPluginName() const = 0;
    
    //==============================================================================
    // Public accessors
    ParameterManager* getParameterManager() const { return parameterManager.get(); }
    AudioEngine* getAudioEngine() const { return audioEngine.get(); }
    PresetManager* getPresetManager() const { return presetManager.get(); }

    void setEffectChainEnabled(bool enabled) { effectChainEnabled = enabled; }
    bool isEffectChainEnabled() const { return effectChainEnabled; }

    void setEffectChainPosition(EffectChainPosition position) { effectChainPosition = position; }
    EffectChainPosition getEffectChainPosition() const { return effectChainPosition; }

protected:
    std::unique_ptr<ParameterManager> parameterManager;
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<PresetManager> presetManager;

    void processEffectChain(juce::AudioBuffer<float>& buffer);

private:
    bool effectChainEnabled = true;
    EffectChainPosition effectChainPosition = EffectChainPosition::PreSource;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveSystemsProcessor)
};