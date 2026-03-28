#include "BandPassFilter.h"

namespace LiveSystems::Components::Filters
{

void BandPassFilter::prepare(double sampleRate, int blockSize)
{
    FilterBase::prepare(sampleRate, blockSize);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 2;

    filter.prepare(spec);
    filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    filter.setCutoffFrequency(frequencyValue);
    filter.setResonance(resonanceValue);
}

void BandPassFilter::process(juce::AudioBuffer<float>& buffer)
{
    filter.setCutoffFrequency(frequencyValue);
    filter.setResonance(resonanceValue);

    if (mixAmount >= 1.0f)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }
    else
    {
        juce::AudioBuffer<float> dryBuffer;
        dryBuffer.makeCopyOf(buffer);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);

        float mix = mixAmount;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* wet = buffer.getWritePointer(ch);
            const auto* dry = dryBuffer.getReadPointer(ch);
            for (int s = 0; s < buffer.getNumSamples(); ++s)
                wet[s] = dry[s] * (1.0f - mix) + wet[s] * mix;
        }
    }
}

void BandPassFilter::reset()
{
    FilterBase::reset();
    filter.reset();
}

} // namespace LiveSystems::Components::Filters
