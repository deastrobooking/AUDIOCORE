#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::AudioEffects
{
    /**
     * @brief Chorus effect using modulated delay lines.
     *
     * Creates a rich, detuned sound by modulating short delay times with an internal LFO.
     * Supports stereo processing with phase-offset modulation between channels.
     */
    class Chorus
    {
    public:
        Chorus() = default;
        ~Chorus() = default;

        void prepare(double sampleRate, int blockSize);
        void process(juce::AudioBuffer<float>& buffer);
        void reset();

        /** Set modulation rate in Hz (0.1–10.0). */
        void setRate(float rateHz);

        /** Set modulation depth in ms (0.0–20.0). */
        void setDepth(float depthMs);

        /** Set centre delay time in ms (1.0–30.0). */
        void setCentreDelay(float delayMs);

        /** Set feedback amount (0.0–0.9). */
        void setFeedback(float feedback);

        /** Set dry/wet mix (0.0–1.0). */
        void setMix(float mix);

    private:
        double currentSampleRate = 44100.0;

        float rate = 1.0f;
        float depth = 3.0f;
        float centreDelay = 7.0f;
        float feedbackAmount = 0.2f;
        float mixAmount = 0.5f;

        // Internal LFO phase
        float lfoPhase = 0.0f;

        static constexpr int maxDelayMs = 50;
        std::vector<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> delayLines;

        float getNextLFOValue(float phaseOffset);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
    };

} // namespace LiveSystems::AudioEffects
