#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

#include "TwelveOperatorSynth.h"
#include "Chorus.h"
#include "SimpleDelay.h"
#include "Reverb.h"
#include "Compressor.h"

//==============================================================================
/**
 * 12-Operator FM Synthesizer Plugin
 *
 * Architecture per voice:
 *   12 Operators → Variable modulation matrix → Per-operator ADSR
 *   → Vactrol envelope → Master HP filter → Master LP filter
 *
 * Master FX chain (post-voice mix):
 *   Chorus → SimpleDelay → Reverb → Compressor
 */
class TwelveOperatorSynthProcessor : public juce::AudioProcessor,
                                     private juce::AudioProcessorValueTreeState::Listener
{
public:
    static constexpr int maxVoices = 4;

    // ---- Parameter IDs ----
    static const juce::String PARAM_MODE;
    static const juce::String PARAM_ALGORITHM;
    static const juce::String PARAM_MASTER_GAIN;
    static const juce::String PARAM_ANALOG_DRIFT;

    static const juce::String PARAM_ATTACK;
    static const juce::String PARAM_DECAY;
    static const juce::String PARAM_SUSTAIN;
    static const juce::String PARAM_RELEASE;

    static const juce::String PARAM_VACTROL_ENABLE;
    static const juce::String PARAM_VACTROL_ATTACK;
    static const juce::String PARAM_VACTROL_RELEASE;
    static const juce::String PARAM_VACTROL_SAG;
    static const juce::String PARAM_VACTROL_CURVE;

    static const juce::String PARAM_HP_CUTOFF;
    static const juce::String PARAM_HP_Q;
    static const juce::String PARAM_LP_CUTOFF;
    static const juce::String PARAM_LP_Q;

    static const juce::String PARAM_CHORUS_ENABLE;
    static const juce::String PARAM_CHORUS_RATE;
    static const juce::String PARAM_CHORUS_DEPTH;
    static const juce::String PARAM_CHORUS_MIX;

    static const juce::String PARAM_DELAY_ENABLE;
    static const juce::String PARAM_DELAY_TIME;
    static const juce::String PARAM_DELAY_FEEDBACK;
    static const juce::String PARAM_DELAY_MIX;

    static const juce::String PARAM_REVERB_ENABLE;
    static const juce::String PARAM_REVERB_SIZE;
    static const juce::String PARAM_REVERB_DAMP;
    static const juce::String PARAM_REVERB_MIX;

    static const juce::String PARAM_COMP_ENABLE;
    static const juce::String PARAM_COMP_THRESHOLD;
    static const juce::String PARAM_COMP_RATIO;

    //==============================================================================
    TwelveOperatorSynthProcessor();
    ~TwelveOperatorSynthProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "12-Op FM Synth"; }
    bool   acceptsMidi()  const override { return true; }
    bool   producesMidi() const override { return false; }
    bool   isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    //==============================================================================
    int  getNumPrograms()   override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    //==============================================================================
    struct SynthVoice
    {
        LiveSystems::Synths::Digital::TwelveOperatorSynth synth;
        int   midiNote  = -1;
        float frequency = 0.0f;
        bool  active    = false;
        int   ageCount  = 0; // samples since noteOn, for voice stealing
    };

    std::array<SynthVoice, maxVoices> voices;

    // Master FX chain
    LiveSystems::AudioEffects::Chorus     chorus;
    LiveSystems::AudioEffects::Delays::SimpleDelay delay;
    LiveSystems::AudioEffects::Reverb     reverb;
    LiveSystems::AudioEffects::Compressor compressor;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    void handleMidiMessage(const juce::MidiMessage& msg);
    void allocateVoice(int midiNote, float velocity);
    void releaseVoice(int midiNote);
    void syncVoiceParameters();

    void parameterChanged(const juce::String& paramID, float newValue) override;

    static float midiNoteToFreq(int note) noexcept;

    double currentSampleRate  = 44100.0;
    int    currentBlockSize   = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwelveOperatorSynthProcessor)
};
