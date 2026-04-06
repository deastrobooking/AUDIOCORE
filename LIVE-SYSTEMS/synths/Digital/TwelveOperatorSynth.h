#pragma once

#include <array>
#include <juce_audio_basics/juce_audio_basics.h>

namespace LiveSystems::Synths::Digital
{
class TwelveOperatorSynth
{
public:
    static constexpr int numOperators = 12;

    enum class Mode
    {
        FM,
        PhaseDistortion,
        VirtualAnalog
    };

    enum class Waveform
    {
        Sine,
        Saw,
        Square,
        Triangle
    };

    struct Operator
    {
        float ratio = 1.0f;
        float detuneHz = 0.0f;
        float level = 0.2f;
        float pan = 0.5f;
        float feedback = 0.0f;
        float phaseDistortion = 0.0f;
        float pulseWidth = 0.5f;
        Waveform waveform = Waveform::Sine;
        bool enabled = true;
    };

    void prepare(double newSampleRate);
    void reset();

    void setMode(Mode newMode);
    Mode getMode() const noexcept { return mode; }

    void setMasterGain(float gainLinear);
    float getMasterGain() const noexcept { return masterGain; }

    void setGlobalAnalogDrift(float amount);
    float getGlobalAnalogDrift() const noexcept { return globalAnalogDrift; }

    void setOperator(int index, const Operator& settings);
    const Operator& getOperator(int index) const;

    void setOperatorLevel(int index, float level);
    void setOperatorRatio(int index, float ratio);
    void setOperatorDetune(int index, float detuneHz);
    void setOperatorPan(int index, float pan);
    void setOperatorFeedback(int index, float feedback);
    void setOperatorPhaseDistortion(int index, float amount);
    void setOperatorPulseWidth(int index, float pulseWidth);
    void setOperatorWaveform(int index, Waveform waveform);
    void setOperatorEnabled(int index, bool enabled);

    void clearModulationMatrix();
    void setModulationIndex(int sourceOperator, int destinationOperator, float index);
    float getModulationIndex(int sourceOperator, int destinationOperator) const;

    void setAlgorithmStacked();
    void setAlgorithmParallel();

    void render(juce::AudioBuffer<float>& buffer,
                int startSample,
                int numSamples,
                float fundamentalHz,
                float amplitude = 1.0f,
                bool addToOutput = false);

private:
    std::array<Operator, numOperators> operators;
    std::array<std::array<float, numOperators>, numOperators> modulationMatrix {};

    std::array<float, numOperators> phases {};
    std::array<float, numOperators> previousOutputs {};
    std::array<float, numOperators> driftState {};

    Mode mode = Mode::FM;
    double sampleRate = 44100.0;
    float masterGain = 0.8f;
    float globalAnalogDrift = 0.0f;

    float processOperatorSample(int opIndex,
                                float fundamentalHz,
                                const std::array<float, numOperators>& currentOutputs);

    static bool isValidOperatorIndex(int index) noexcept;
    static float applyWaveform(Waveform waveform, float phase, float pulseWidth);
    static float applyPhaseDistortion(float phase, float amount);
    static float wrapPhase(float phase);
};
} // namespace LiveSystems::Synths::Digital
