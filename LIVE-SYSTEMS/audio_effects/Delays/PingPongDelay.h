#pragma once

#include "DelayBase.h"

namespace LiveSystems::AudioEffects::Delays
{
    /**
     * @brief Ping-pong delay that alternates echoes between left and right channels.
     *
     * Requires stereo input. For mono input, the output is duplicated to both channels.
     */
    class PingPongDelay : public DelayBase
    {
    public:
        PingPongDelay() = default;
        ~PingPongDelay() override = default;

        void prepare(double sampleRate, int blockSize) override;
        void process(juce::AudioBuffer<float>& buffer) override;
        void reset() override;

    private:
        static constexpr int maxDelaySeconds = 2;
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLeft { 0 };
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayRight { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PingPongDelay)
    };

} // namespace LiveSystems::AudioEffects::Delays
