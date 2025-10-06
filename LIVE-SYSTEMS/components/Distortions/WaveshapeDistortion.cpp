#include "WaveshapeDistortion.h"
#include <cmath>

namespace LiveSystems::Components::Distortions
{
    void WaveshapeDistortion::process(juce::AudioBuffer<float>& buffer)
    {
        auto numChannels = buffer.getNumChannels();
        auto numSamples = buffer.getNumSamples();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                auto drive = driveSmoothed.getNextValue();
                auto mix = mixSmoothed.getNextValue();
                auto output = outputSmoothed.getNextValue();

                auto inputSample = channelData[sample];
                auto wetSample = std::tanh(inputSample * (1.0f + drive * 9.0f));
                auto mixedSample = (inputSample * (1.0f - mix)) + (wetSample * mix);
                channelData[sample] = mixedSample * output;
            }
        }
    }
}
