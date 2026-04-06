#include "Reverb.h"

namespace LiveSystems::AudioEffects
{

void Reverb::prepare(double sampleRate, int blockSize)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels      = 2;

    reverb.prepare(spec);
    reverb.setParameters(params);
}

void Reverb::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    if (numSamples == 0) return;

    if (numChannels >= 2)
    {
        // True stereo path
        juce::dsp::AudioBlock<float> block(buffer);
        reverb.process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    else if (numChannels == 1)
    {
        // Mono: upmix to stereo temp buffer, process, downmix back
        juce::AudioBuffer<float> stereoBuffer(2, numSamples);
        stereoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        stereoBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);

        juce::dsp::AudioBlock<float> block(stereoBuffer);
        reverb.process(juce::dsp::ProcessContextReplacing<float>(block));

        for (int s = 0; s < numSamples; ++s)
            buffer.setSample(0, s, 0.5f * (stereoBuffer.getSample(0, s)
                                          + stereoBuffer.getSample(1, s)));
    }
}

void Reverb::reset()
{
    reverb.reset();
}

void Reverb::setRoomSize(float size)
{
    params.roomSize = juce::jlimit(0.0f, 1.0f, size);
    reverb.setParameters(params);
}

void Reverb::setDamping(float damping)
{
    params.damping = juce::jlimit(0.0f, 1.0f, damping);
    reverb.setParameters(params);
}

void Reverb::setWidth(float width)
{
    params.width = juce::jlimit(0.0f, 1.0f, width);
    reverb.setParameters(params);
}

void Reverb::setWetLevel(float wet)
{
    params.wetLevel = juce::jlimit(0.0f, 1.0f, wet);
    reverb.setParameters(params);
}

void Reverb::setDryLevel(float dry)
{
    params.dryLevel = juce::jlimit(0.0f, 1.0f, dry);
    reverb.setParameters(params);
}

void Reverb::setFreezeMode(bool frozen)
{
    params.freezeMode = frozen ? 1.0f : 0.0f;
    reverb.setParameters(params);
}

} // namespace LiveSystems::AudioEffects
