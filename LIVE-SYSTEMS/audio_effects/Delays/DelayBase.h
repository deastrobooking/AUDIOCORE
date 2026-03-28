#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::AudioEffects::Delays
{
    /**
     * @brief Base class for all delay effects.
     *
     * Provides common interface, delay line management, and smoothed parameters.
     */
    class DelayBase
    {
    public:
        DelayBase() = default;
        virtual ~DelayBase() = default;

        virtual void prepare(double sampleRate, int blockSize);
        virtual void process(juce::AudioBuffer<float>& buffer) = 0;
        virtual void reset();

        /** Set delay time in milliseconds (0–2000 ms). */
        virtual void setDelayTime(float timeMs);

        /** Set feedback amount (0.0–0.95). Clamped to prevent runaway feedback. */
        virtual void setFeedback(float feedback);

        /** Set dry/wet mix (0.0 = dry, 1.0 = wet). */
        virtual void setMix(float mix);

    protected:
        double currentSampleRate = 44100.0;
        int currentBlockSize = 512;

        float delayTimeMs = 300.0f;
        float feedbackAmount = 0.3f;
        float mixAmount = 0.5f;

        juce::SmoothedValue<float> delayTimeSmoothed;
        juce::SmoothedValue<float> feedbackSmoothed;
        juce::SmoothedValue<float> mixSmoothed;

        /** Convert delay time in ms to samples. */
        float msToSamples(float ms) const
        {
            return static_cast<float>(ms * currentSampleRate / 1000.0);
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayBase)
    };

} // namespace LiveSystems::AudioEffects::Delays
