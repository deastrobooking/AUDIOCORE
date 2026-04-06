#include "../include/AudioEngine.h"
#include <cmath>

//==============================================================================
AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
}

//==============================================================================
void AudioEngine::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    blockSize = static_cast<int>(spec.maximumBlockSize);
    numChannels = static_cast<int>(spec.numChannels);
    
    // Prepare built-in processors
    filter.prepare(spec);
    compressor.prepare(spec);
    reverb.prepare(spec);
    
    // Prepare custom processor chain
    for (auto& processor : processorChain)
        processor->prepare(spec);
}

void AudioEngine::process(juce::AudioBuffer<float>& buffer)
{
    auto startTime = juce::Time::getHighResolutionTicks();

    // Built-in processing chain
    filter.process(buffer);
    compressor.process(buffer);
    reverb.process(buffer);
    
    // Process through custom processor chain
    for (auto& processor : processorChain)
        processor->process(buffer);
    
    auto endTime = juce::Time::getHighResolutionTicks();
    auto elapsedSeconds = juce::Time::highResolutionTicksToSeconds(endTime - startTime);
    auto blockDurationSeconds = buffer.getNumSamples() > 0 ? (static_cast<double>(buffer.getNumSamples()) / sampleRate) : 0.0;
    if (blockDurationSeconds > 0.0)
        updateCpuUsage((elapsedSeconds / blockDurationSeconds) * 100.0);
}

void AudioEngine::reset()
{
    filter.reset();
    compressor.reset();
    reverb.reset();
    
    for (auto& processor : processorChain)
        processor->reset();
}

//==============================================================================
void AudioEngine::removeProcessor(int index)
{
    if (index >= 0 && index < static_cast<int>(processorChain.size()))
        processorChain.erase(processorChain.begin() + index);
}

void AudioEngine::clearProcessors()
{
    processorChain.clear();
}

//==============================================================================
// Filter Implementation
void AudioEngine::Filter::setType(Type type)
{
    currentType = type;
    switch (type)
    {
        case LowPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
        case HighPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            break;
        case BandPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
        case Notch:
            updateNotchCoefficients();
            break;
    }
}

void AudioEngine::Filter::setFrequency(float frequency)
{
    currentFrequency = frequency;
    filter.setCutoffFrequency(frequency);
    updateNotchCoefficients();
}

void AudioEngine::Filter::setResonance(float resonance)
{
    currentResonance = resonance;
    filter.setResonance(resonance);
    updateNotchCoefficients();
}

void AudioEngine::Filter::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    filter.prepare(spec);
    notchFilter.prepare(spec);
    updateNotchCoefficients();
}

void AudioEngine::Filter::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (currentType == Notch)
        notchFilter.process(context);
    else
        filter.process(context);
}

void AudioEngine::Filter::reset()
{
    filter.reset();
    notchFilter.reset();
}

void AudioEngine::Filter::updateNotchCoefficients()
{
    if (sampleRate <= 0.0)
        return;

    const auto safeFrequency = juce::jlimit(20.0f, static_cast<float>(0.49 * sampleRate), currentFrequency);
    const auto safeQ = juce::jlimit(0.1f, 20.0f, currentResonance);
    *notchFilter.state = *juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, safeFrequency, safeQ);
}

//==============================================================================
// Compressor Implementation
void AudioEngine::Compressor::setThreshold(float thresholdDb)
{
    compressor.setThreshold(thresholdDb);
}

void AudioEngine::Compressor::setRatio(float ratio)
{
    compressor.setRatio(ratio);
}

void AudioEngine::Compressor::setAttack(float attackMs)
{
    compressor.setAttack(attackMs);
}

void AudioEngine::Compressor::setRelease(float releaseMs)
{
    compressor.setRelease(releaseMs);
}

void AudioEngine::Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void AudioEngine::Compressor::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    compressor.process(context);
}

void AudioEngine::Compressor::reset()
{
    compressor.reset();
}

//==============================================================================
// Reverb Implementation
void AudioEngine::Reverb::setRoomSize(float size)
{
    juce::dsp::Reverb::Parameters params;
    params.roomSize = size;
    reverb.setParameters(params);
}

void AudioEngine::Reverb::setDamping(float damping)
{
    juce::dsp::Reverb::Parameters params = reverb.getParameters();
    params.damping = damping;
    reverb.setParameters(params);
}

void AudioEngine::Reverb::setWetLevel(float wetLevel)
{
    juce::dsp::Reverb::Parameters params = reverb.getParameters();
    params.wetLevel = wetLevel;
    reverb.setParameters(params);
}

void AudioEngine::Reverb::setDryLevel(float dryLevel)
{
    juce::dsp::Reverb::Parameters params = reverb.getParameters();
    params.dryLevel = dryLevel;
    reverb.setParameters(params);
}

void AudioEngine::Reverb::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);
}

void AudioEngine::Reverb::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void AudioEngine::Reverb::reset()
{
    reverb.reset();
}