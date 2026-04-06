#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::AudioEffects
{

/**
 * @brief Stereo reverb effect wrapping juce::dsp::Reverb (Freeverb algorithm).
 *
 * Controls: room size, damping, width, wet/dry mix, and freeze mode.
 */
class Reverb
{
public:
    Reverb() = default;
    ~Reverb() = default;

    void prepare(double sampleRate, int blockSize);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    /** Room size / decay length [0.0–1.0]. */
    void setRoomSize(float size);

    /** High-frequency damping [0.0–1.0]. */
    void setDamping(float damping);

    /** Stereo width [0.0–1.0]. */
    void setWidth(float width);

    /** Wet signal level [0.0–1.0]. */
    void setWetLevel(float wet);

    /** Dry signal level [0.0–1.0]. */
    void setDryLevel(float dry);

    /** Freeze – infinite sustain when true. */
    void setFreezeMode(bool frozen);

    float getRoomSize()  const noexcept { return params.roomSize; }
    float getDamping()   const noexcept { return params.damping;  }
    float getWidth()     const noexcept { return params.width;    }
    float getWetLevel()  const noexcept { return params.wetLevel; }
    float getDryLevel()  const noexcept { return params.dryLevel; }

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};

} // namespace LiveSystems::AudioEffects
