#pragma once

#include "DelayBase.h"

namespace LiveSystems::AudioEffects::Delays
{
    /**
     * @brief Simple mono/stereo delay with feedback.
     *
     * Each channel has its own independent delay line.
     * Uses linear interpolation for smooth delay time changes.
     */
    class SimpleDelay : public DelayBase
    {
    public:
        SimpleDelay() = default;
        ~SimpleDelay() override = default;

        void prepare(double sampleRate, int blockSize) override;
        void process(juce::AudioBuffer<float>& buffer) override;
        void reset() override;

    private:
        static constexpr int maxDelaySeconds = 2;
        std::vector<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> delayLines;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDelay)
    };

} // namespace LiveSystems::AudioEffects::Delays
