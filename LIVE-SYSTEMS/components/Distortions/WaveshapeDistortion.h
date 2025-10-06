#pragma once

#include "DistortionBase.h"

namespace LiveSystems::Components::Distortions
{
    class WaveshapeDistortion : public DistortionBase
    {
    public:
        WaveshapeDistortion() = default;
        ~WaveshapeDistortion() override = default;

        void process(juce::AudioBuffer<float>& buffer) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshapeDistortion)
    };
}
