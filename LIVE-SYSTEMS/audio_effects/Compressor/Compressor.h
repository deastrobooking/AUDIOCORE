#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::AudioEffects
{

class Compressor
{
public:
    Compressor() = default;
    ~Compressor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        compressor.prepare(spec);
        compressor.setThreshold(thresholdDb);
        compressor.setRatio(ratio);
        compressor.setAttack(attackMs);
        compressor.setRelease(releaseMs);

        makeupGainSmoothed.reset(sampleRate, 0.02);
        mixSmoothed.reset(sampleRate, 0.02);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumSamples() == 0) return;

        // Store dry signal for mix
        dryBuffer.makeCopyOf(buffer);

        // Apply compression
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        compressor.process(context);

        // Apply makeup gain and dry/wet mix
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float gain = juce::Decibels::decibelsToGain(makeupGainSmoothed.getNextValue());
            float mix = mixSmoothed.getNextValue();

            for (int ch = 0; ch < numChannels; ++ch)
            {
                float wet = buffer.getSample(ch, sample) * gain;
                float dry = dryBuffer.getSample(ch, sample);
                buffer.setSample(ch, sample, dry + mix * (wet - dry));
            }
        }
    }

    void reset()
    {
        compressor.reset();
        makeupGainSmoothed.setCurrentAndTargetValue(makeupGainDb);
        mixSmoothed.setCurrentAndTargetValue(mix);
    }

    void setThreshold(float dB)
    {
        thresholdDb = juce::jlimit(-60.0f, 0.0f, dB);
        compressor.setThreshold(thresholdDb);
    }

    void setRatio(float newRatio)
    {
        ratio = juce::jlimit(1.0f, 20.0f, newRatio);
        compressor.setRatio(ratio);
    }

    void setAttack(float ms)
    {
        attackMs = juce::jlimit(0.1f, 200.0f, ms);
        compressor.setAttack(attackMs);
    }

    void setRelease(float ms)
    {
        releaseMs = juce::jlimit(5.0f, 2000.0f, ms);
        compressor.setRelease(releaseMs);
    }

    void setMakeupGain(float dB)
    {
        makeupGainDb = juce::jlimit(0.0f, 40.0f, dB);
        makeupGainSmoothed.setTargetValue(makeupGainDb);
    }

    void setMix(float newMix)
    {
        mix = juce::jlimit(0.0f, 1.0f, newMix);
        mixSmoothed.setTargetValue(mix);
    }

    float getThreshold() const { return thresholdDb; }
    float getRatio() const { return ratio; }
    float getAttack() const { return attackMs; }
    float getRelease() const { return releaseMs; }
    float getMakeupGain() const { return makeupGainDb; }
    float getMix() const { return mix; }

private:
    juce::dsp::Compressor<float> compressor;
    juce::AudioBuffer<float> dryBuffer;

    double sampleRate = 44100.0;

    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float makeupGainDb = 0.0f;
    float mix = 1.0f;

    juce::SmoothedValue<float> makeupGainSmoothed { 0.0f };
    juce::SmoothedValue<float> mixSmoothed { 1.0f };
};

} // namespace LiveSystems::AudioEffects
