#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::Components::Oscillators
{
    /**
     * @brief Low Frequency Oscillator for modulation
     * 
     * Provides various waveform types for modulating parameters.
     */
    class LFO
    {
    public:
        enum class Waveform
        {
            Sine,
            Triangle,
            Square,
            Saw,
            ReverseSaw,
            Random
        };

        LFO() = default;
        ~LFO() = default;

        /**
         * @brief Prepare the LFO for processing
         * @param sampleRate The sample rate to use
         */
        void prepare(double sampleRate);

        /**
         * @brief Reset the LFO phase
         */
        void reset();

        /**
         * @brief Get the next LFO sample
         * @return LFO value in range [0.0, 1.0]
         */
        float getNextSample();

        /**
         * @brief Set the LFO frequency in Hz
         * @param frequencyHz Frequency in Hz (0.01 to 20.0)
         */
        void setFrequency(float frequencyHz);

        /**
         * @brief Set the LFO waveform type
         * @param waveform The waveform to use
         */
        void setWaveform(Waveform waveform);

        /**
         * @brief Set the LFO phase offset
         * @param phaseOffset Phase offset in radians (0 to 2*PI)
         */
        void setPhaseOffset(float phaseOffset);

        /**
         * @brief Get current waveform type
         */
        Waveform getWaveform() const { return currentWaveform; }

    private:
        double sampleRate = 44100.0;
        double phase = 0.0;
        double phaseIncrement = 0.0;
        float phaseOffset = 0.0f;
        float frequency = 1.0f;
        Waveform currentWaveform = Waveform::Sine;
        juce::Random random;

        float generateWaveform();
        float generateSine();
        float generateTriangle();
        float generateSquare();
        float generateSaw();
        float generateReverseSaw();
        float generateRandom();

        // Random state
        float lastRandom = 0.5f;
        double lastPhase = 0.0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFO)
    };
}
