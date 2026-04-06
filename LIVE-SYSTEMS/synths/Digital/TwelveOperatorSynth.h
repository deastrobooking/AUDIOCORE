#pragma once

#include <array>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

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

    /// Named algorithm presets for the 12×12 modulation matrix.
    enum class AlgorithmPreset
    {
        Custom,       ///< Free matrix – user-defined connections
        Stacked,      ///< Linear cascade: op11→…→op0 (single carrier)
        Parallel,     ///< All ops modulate op 0 independently
        DualStack,    ///< Two cascades of 6 in parallel
        TripletFM,    ///< Three groups of 4 (3 modulator+carrier chains)
        FeedbackChain,///< Cascade with self-feedback on every operator
        StarBurst,    ///< Op11 modulates all others; all are carriers
        Layered,      ///< Upper hex (6-11) modulates lower hex (0-5) in pairs
    };

    /// Per-operator ADSR envelope.
    struct ADSRSettings
    {
        float attackMs  = 5.0f;
        float decayMs   = 100.0f;
        float sustain   = 0.7f;
        float releaseMs = 200.0f;
    };

    /// Models an optocoupler (LED+LDR): fast attack, slow organic release.
    struct VactrolSettings
    {
        bool  enabled   = false;
        float attackMs  = 2.0f;   ///< Fast optical attack
        float releaseMs = 400.0f; ///< Slow LDR exponential release
        float sag       = 0.15f;  ///< Release slowdown on repeated triggers [0,1]
        float curve     = 0.6f;   ///< Nonlinear LDR curve [0=linear, 1=full-log]
    };

    struct Operator
    {
        float ratio           = 1.0f;
        float detuneHz        = 0.0f;
        float level           = 0.2f;
        float pan             = 0.5f;
        float feedback        = 0.0f;
        float phaseDistortion = 0.0f;
        float pulseWidth      = 0.5f;
        float subLevel        = 0.0f;  ///< Sub-oscillator blend (VirtualAnalog only)
        Waveform   waveform   = Waveform::Sine;
        ADSRSettings adsr;
        bool enabled          = true;
    };

    // ---- Lifecycle --------------------------------------------------------
    void prepare(double newSampleRate);
    void reset();

    // ---- Note control (triggers all operator envelopes) -------------------
    void noteOn (float velocity = 1.0f);
    void noteOff();
    bool isActive() const noexcept;

    // ---- Mode -------------------------------------------------------------
    void setMode(Mode newMode);
    Mode getMode() const noexcept { return mode; }

    // ---- Master gain ------------------------------------------------------
    void  setMasterGain(float gainLinear);
    float getMasterGain() const noexcept { return masterGain; }

    // ---- Analog drift (VirtualAnalog) -------------------------------------
    void  setGlobalAnalogDrift(float amount);
    float getGlobalAnalogDrift() const noexcept { return globalAnalogDrift; }

    // ---- Per-operator -----------------------------------------------------
    void            setOperator              (int index, const Operator& settings);
    const Operator& getOperator              (int index) const;

    void setOperatorLevel          (int index, float level);
    void setOperatorRatio          (int index, float ratio);
    void setOperatorDetune         (int index, float detuneHz);
    void setOperatorPan            (int index, float pan);
    void setOperatorFeedback       (int index, float feedback);
    void setOperatorPhaseDistortion(int index, float amount);
    void setOperatorPulseWidth     (int index, float pulseWidth);
    void setOperatorSubLevel       (int index, float subLevel);
    void setOperatorWaveform       (int index, Waveform waveform);
    void setOperatorEnabled        (int index, bool enabled);
    void setOperatorADSR           (int index, const ADSRSettings& adsr);

    /// Apply the same ADSR to every operator at once.
    void setGlobalADSR(const ADSRSettings& adsr);

    // ---- Vactrol envelope -------------------------------------------------
    void                   setVactrolSettings(const VactrolSettings& settings);
    const VactrolSettings& getVactrolSettings() const noexcept { return vactrol; }

    // ---- Master filter chain: High-pass → Low-pass ------------------------
    void setHighPassCutoff   (float frequencyHz);
    void setHighPassResonance(float q);
    void setLowPassCutoff    (float frequencyHz);
    void setLowPassResonance (float q);
    void setFiltersEnabled   (bool  enabled);

    float getHighPassCutoff()    const noexcept { return hpCutoff; }
    float getHighPassResonance() const noexcept { return hpResonance; }
    float getLowPassCutoff()     const noexcept { return lpCutoff; }
    float getLowPassResonance()  const noexcept { return lpResonance; }

    // ---- Modulation matrix ------------------------------------------------
    void  clearModulationMatrix();
    void  setModulationIndex(int sourceOperator, int destinationOperator, float index);
    float getModulationIndex(int sourceOperator, int destinationOperator) const;

    // ---- Algorithm presets ------------------------------------------------
    void            setAlgorithmPreset(AlgorithmPreset preset);
    AlgorithmPreset getCurrentAlgorithm() const noexcept { return currentAlgorithm; }

    // ---- Render -----------------------------------------------------------
    void render(juce::AudioBuffer<float>& buffer,
                int   startSample,
                int   numSamples,
                float fundamentalHz,
                float amplitude   = 1.0f,
                bool  addToOutput = false);

private:
    // ---- Operator & matrix state ------------------------------------------
    std::array<Operator, numOperators> operators;
    std::array<std::array<float, numOperators>, numOperators> modulationMatrix {};

    std::array<float, numOperators> phases {};
    std::array<float, numOperators> previousOutputs {};
    std::array<float, numOperators> driftState {};

    // ---- Per-operator ADSR state ------------------------------------------
    enum class EnvelopeStage { Idle, Attack, Decay, Sustain, Release };
    struct EnvelopeState
    {
        EnvelopeStage stage    = EnvelopeStage::Idle;
        float         value    = 0.0f;
        float         velocity = 1.0f;
    };
    std::array<EnvelopeState, numOperators> envelopeStates {};

    // ---- Vactrol state ----------------------------------------------------
    struct VactrolState { float value = 0.0f; float sagAccum = 0.0f; };
    VactrolState    vactrolState;
    VactrolSettings vactrol;

    // ---- Master filter chain ----------------------------------------------
    using SVFilter = juce::dsp::StateVariableTPTFilter<float>;
    SVFilter highpassFilter;
    SVFilter lowpassFilter;
    float    hpCutoff      = 20.0f;
    float    hpResonance   = 0.707f;
    float    lpCutoff      = 20000.0f;
    float    lpResonance   = 0.707f;
    bool     filtersEnabled = true;

    // ---- General state ----------------------------------------------------
    Mode            mode              = Mode::FM;
    AlgorithmPreset currentAlgorithm  = AlgorithmPreset::Custom;
    double          sampleRate        = 44100.0;
    float           masterGain        = 0.8f;
    float           globalAnalogDrift = 0.0f;
    bool            noteIsOn          = false;

    // Per-instance random – avoids contention on juce::Random::getSystemRandom()
    juce::Random rng { juce::Time::currentTimeMillis() };

    // ---- Private helpers --------------------------------------------------
    float processOperatorSample(int opIndex,
                                float fundamentalHz,
                                const std::array<float, numOperators>& currentOutputs);
    float tickEnvelope(int opIndex);
    float tickVactrol ();

    static bool  isValidOperatorIndex (int index) noexcept;
    static float applyWaveform        (Waveform waveform, float phase, float pulseWidth);
    static float applyPhaseDistortion (float phase, float amount);
    static float wrapPhase            (float phase) noexcept;
};
} // namespace LiveSystems::Synths::Digital
