#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>

//==============================================================================
/**
    High-performance audio processing engine for LIVE-SYSTEMS plugins.
    Provides DSP utilities, effect chains, and optimized audio processing.
*/
class AudioEngine
{
public:
    //==============================================================================
    AudioEngine();
    ~AudioEngine();

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    //==============================================================================
    // DSP Chain Management
    template<typename ProcessorType>
    void addProcessor(std::unique_ptr<ProcessorType> processor);
    
    void removeProcessor(int index);
    void clearProcessors();

    //==============================================================================
    // Common DSP utilities
    class Filter
    {
    public:
        enum Type { LowPass, HighPass, BandPass, Notch };
        
        void setType(Type type);
        void setFrequency(float frequency);
        void setResonance(float resonance);
        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::AudioBuffer<float>& buffer);
        void reset();

    private:
        juce::dsp::StateVariableTPTFilter<float> filter;
        Type currentType = LowPass;
    };

    class Compressor
    {
    public:
        void setThreshold(float thresholdDb);
        void setRatio(float ratio);
        void setAttack(float attackMs);
        void setRelease(float releaseMs);
        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::AudioBuffer<float>& buffer);
        void reset();

    private:
        juce::dsp::Compressor<float> compressor;
    };

    class Reverb
    {
    public:
        void setRoomSize(float size);
        void setDamping(float damping);
        void setWetLevel(float wetLevel);
        void setDryLevel(float dryLevel);
        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::AudioBuffer<float>& buffer);
        void reset();

    private:
        juce::dsp::Reverb reverb;
    };

    //==============================================================================
    // Built-in processors
    Filter& getFilter() { return filter; }
    Compressor& getCompressor() { return compressor; }
    Reverb& getReverb() { return reverb; }

    //==============================================================================
    // Performance monitoring
    double getCpuUsage() const { return cpuUsage; }
    void updateCpuUsage(double usage) { cpuUsage = usage; }

private:
    //==============================================================================
    // Processor chain for custom effects
    class ProcessorChain
    {
    public:
        virtual ~ProcessorChain() = default;
        virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
        virtual void process(juce::AudioBuffer<float>& buffer) = 0;
        virtual void reset() = 0;
    };

    std::vector<std::unique_ptr<ProcessorChain>> processorChain;
    
    Filter filter;
    Compressor compressor;
    Reverb reverb;
    
    double sampleRate = 44100.0;
    int blockSize = 512;
    int numChannels = 2;
    double cpuUsage = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};