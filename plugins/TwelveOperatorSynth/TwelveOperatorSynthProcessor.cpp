#include "TwelveOperatorSynthProcessor.h"
#include "TwelveOperatorSynthEditor.h"

using namespace LiveSystems::Synths::Digital;
using namespace LiveSystems::AudioEffects;

// ============================================================================
// Parameter ID constants
// ============================================================================
const juce::String TwelveOperatorSynthProcessor::PARAM_MODE           = "mode";
const juce::String TwelveOperatorSynthProcessor::PARAM_ALGORITHM      = "algorithm";
const juce::String TwelveOperatorSynthProcessor::PARAM_MASTER_GAIN    = "masterGain";
const juce::String TwelveOperatorSynthProcessor::PARAM_ANALOG_DRIFT   = "analogDrift";

const juce::String TwelveOperatorSynthProcessor::PARAM_ATTACK         = "attack";
const juce::String TwelveOperatorSynthProcessor::PARAM_DECAY          = "decay";
const juce::String TwelveOperatorSynthProcessor::PARAM_SUSTAIN        = "sustain";
const juce::String TwelveOperatorSynthProcessor::PARAM_RELEASE        = "release";

const juce::String TwelveOperatorSynthProcessor::PARAM_VACTROL_ENABLE  = "vactrolEnable";
const juce::String TwelveOperatorSynthProcessor::PARAM_VACTROL_ATTACK  = "vactrolAttack";
const juce::String TwelveOperatorSynthProcessor::PARAM_VACTROL_RELEASE = "vactrolRelease";
const juce::String TwelveOperatorSynthProcessor::PARAM_VACTROL_SAG    = "vactrolSag";
const juce::String TwelveOperatorSynthProcessor::PARAM_VACTROL_CURVE  = "vactrolCurve";

const juce::String TwelveOperatorSynthProcessor::PARAM_HP_CUTOFF      = "hpCutoff";
const juce::String TwelveOperatorSynthProcessor::PARAM_HP_Q           = "hpQ";
const juce::String TwelveOperatorSynthProcessor::PARAM_LP_CUTOFF      = "lpCutoff";
const juce::String TwelveOperatorSynthProcessor::PARAM_LP_Q           = "lpQ";

const juce::String TwelveOperatorSynthProcessor::PARAM_CHORUS_ENABLE  = "chorusEnable";
const juce::String TwelveOperatorSynthProcessor::PARAM_CHORUS_RATE    = "chorusRate";
const juce::String TwelveOperatorSynthProcessor::PARAM_CHORUS_DEPTH   = "chorusDepth";
const juce::String TwelveOperatorSynthProcessor::PARAM_CHORUS_MIX     = "chorusMix";

const juce::String TwelveOperatorSynthProcessor::PARAM_DELAY_ENABLE   = "delayEnable";
const juce::String TwelveOperatorSynthProcessor::PARAM_DELAY_TIME     = "delayTime";
const juce::String TwelveOperatorSynthProcessor::PARAM_DELAY_FEEDBACK = "delayFeedback";
const juce::String TwelveOperatorSynthProcessor::PARAM_DELAY_MIX      = "delayMix";

const juce::String TwelveOperatorSynthProcessor::PARAM_REVERB_ENABLE  = "reverbEnable";
const juce::String TwelveOperatorSynthProcessor::PARAM_REVERB_SIZE    = "reverbSize";
const juce::String TwelveOperatorSynthProcessor::PARAM_REVERB_DAMP   = "reverbDamp";
const juce::String TwelveOperatorSynthProcessor::PARAM_REVERB_MIX    = "reverbMix";

const juce::String TwelveOperatorSynthProcessor::PARAM_COMP_ENABLE    = "compEnable";
const juce::String TwelveOperatorSynthProcessor::PARAM_COMP_THRESHOLD = "compThreshold";
const juce::String TwelveOperatorSynthProcessor::PARAM_COMP_RATIO     = "compRatio";

// ============================================================================
// Parameter layout
// ============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
TwelveOperatorSynthProcessor::createParameterLayout()
{
    using APF  = juce::AudioParameterFloat;
    using APC  = juce::AudioParameterChoice;
    using APB  = juce::AudioParameterBool;
    using Range = juce::NormalisableRange<float>;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Synthesis
    layout.add(std::make_unique<APC>(PARAM_MODE, "Mode",
        juce::StringArray { "FM", "Phase Distortion", "Virtual Analog" }, 0));
    layout.add(std::make_unique<APC>(PARAM_ALGORITHM, "Algorithm",
        juce::StringArray { "Custom", "Stacked", "Parallel", "Dual Stack",
                            "Triplet FM", "Feedback Chain", "Starburst", "Layered" }, 1));
    layout.add(std::make_unique<APF>(PARAM_MASTER_GAIN, "Master Gain",
        Range(0.0f, 1.0f, 0.001f), 0.8f));
    layout.add(std::make_unique<APF>(PARAM_ANALOG_DRIFT, "Analog Drift",
        Range(0.0f, 0.2f, 0.001f), 0.0f));

    // ADSR
    layout.add(std::make_unique<APF>(PARAM_ATTACK,  "Attack",   Range(0.5f, 2000.0f, 0.1f, 0.4f), 5.0f));
    layout.add(std::make_unique<APF>(PARAM_DECAY,   "Decay",    Range(1.0f, 2000.0f, 0.1f, 0.4f), 100.0f));
    layout.add(std::make_unique<APF>(PARAM_SUSTAIN, "Sustain",  Range(0.0f, 1.0f, 0.001f), 0.7f));
    layout.add(std::make_unique<APF>(PARAM_RELEASE, "Release",  Range(1.0f, 5000.0f, 0.1f, 0.4f), 200.0f));

    // Vactrol
    layout.add(std::make_unique<APB>(PARAM_VACTROL_ENABLE,  "Vactrol Enable",  false));
    layout.add(std::make_unique<APF>(PARAM_VACTROL_ATTACK,  "Vactrol Attack",  Range(0.5f, 50.0f,   0.1f), 2.0f));
    layout.add(std::make_unique<APF>(PARAM_VACTROL_RELEASE, "Vactrol Release", Range(50.0f, 2000.0f, 0.1f, 0.4f), 400.0f));
    layout.add(std::make_unique<APF>(PARAM_VACTROL_SAG,     "Vactrol Sag",     Range(0.0f, 1.0f, 0.001f), 0.15f));
    layout.add(std::make_unique<APF>(PARAM_VACTROL_CURVE,   "Vactrol Curve",   Range(0.0f, 1.0f, 0.001f), 0.6f));

    // Filters
    layout.add(std::make_unique<APF>(PARAM_HP_CUTOFF, "HP Cutoff",
        Range(20.0f, 2000.0f, 0.1f, 0.4f), 20.0f));
    layout.add(std::make_unique<APF>(PARAM_HP_Q, "HP Resonance",
        Range(0.3f, 4.0f, 0.01f), 0.707f));
    layout.add(std::make_unique<APF>(PARAM_LP_CUTOFF, "LP Cutoff",
        Range(200.0f, 20000.0f, 0.1f, 0.3f), 20000.0f));
    layout.add(std::make_unique<APF>(PARAM_LP_Q, "LP Resonance",
        Range(0.3f, 4.0f, 0.01f), 0.707f));

    // Chorus
    layout.add(std::make_unique<APB>(PARAM_CHORUS_ENABLE, "Chorus Enable", false));
    layout.add(std::make_unique<APF>(PARAM_CHORUS_RATE,   "Chorus Rate",   Range(0.1f, 10.0f,  0.01f), 1.0f));
    layout.add(std::make_unique<APF>(PARAM_CHORUS_DEPTH,  "Chorus Depth",  Range(0.0f, 20.0f,  0.01f), 3.0f));
    layout.add(std::make_unique<APF>(PARAM_CHORUS_MIX,    "Chorus Mix",    Range(0.0f,  1.0f, 0.001f), 0.5f));

    // Delay
    layout.add(std::make_unique<APB>(PARAM_DELAY_ENABLE,   "Delay Enable",   false));
    layout.add(std::make_unique<APF>(PARAM_DELAY_TIME,     "Delay Time",     Range(10.0f, 2000.0f, 0.1f, 0.4f), 300.0f));
    layout.add(std::make_unique<APF>(PARAM_DELAY_FEEDBACK, "Delay Feedback", Range(0.0f, 0.95f, 0.001f), 0.3f));
    layout.add(std::make_unique<APF>(PARAM_DELAY_MIX,      "Delay Mix",      Range(0.0f, 1.0f, 0.001f), 0.3f));

    // Reverb
    layout.add(std::make_unique<APB>(PARAM_REVERB_ENABLE, "Reverb Enable", false));
    layout.add(std::make_unique<APF>(PARAM_REVERB_SIZE,   "Reverb Size",   Range(0.0f, 1.0f, 0.001f), 0.5f));
    layout.add(std::make_unique<APF>(PARAM_REVERB_DAMP,  "Reverb Damp",   Range(0.0f, 1.0f, 0.001f), 0.5f));
    layout.add(std::make_unique<APF>(PARAM_REVERB_MIX,   "Reverb Mix",    Range(0.0f, 1.0f, 0.001f), 0.3f));

    // Compressor
    layout.add(std::make_unique<APB>(PARAM_COMP_ENABLE,    "Comp Enable",    false));
    layout.add(std::make_unique<APF>(PARAM_COMP_THRESHOLD, "Comp Threshold",
        Range(-60.0f, 0.0f, 0.1f), -12.0f));
    layout.add(std::make_unique<APF>(PARAM_COMP_RATIO, "Comp Ratio",
        Range(1.0f, 20.0f, 0.01f), 4.0f));

    return layout;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
TwelveOperatorSynthProcessor::TwelveOperatorSynthProcessor()
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "TwelveOpFMSynth", createParameterLayout())
{
    // Register as parameter listener
    for (auto* param : apvts.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.addParameterListener(ranged->getParameterID(), this);
}

TwelveOperatorSynthProcessor::~TwelveOperatorSynthProcessor()
{
    for (auto* param : apvts.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.removeParameterListener(ranged->getParameterID(), this);
}

// ============================================================================
// Prepare / Release
// ============================================================================
void TwelveOperatorSynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = samplesPerBlock;

    for (auto& v : voices)
    {
        v.synth.prepare(sampleRate);
        v.synth.setFiltersEnabled(false); // Global filters applied per-voice in synth,
                                          // but master HP/LP are set via syncVoiceParameters
        v.active   = false;
        v.midiNote = -1;
        v.ageCount = 0;
    }

    chorus.prepare(sampleRate, samplesPerBlock);

    delay.prepare(sampleRate, samplesPerBlock);

    reverb.prepare(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels      = 2;
    compressor.prepare(spec);

    syncVoiceParameters();
}

void TwelveOperatorSynthProcessor::releaseResources()
{
    for (auto& v : voices)
        v.synth.reset();
}

bool TwelveOperatorSynthProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (!layouts.getMainInputChannelSet().isDisabled())
        return false;
    return true;
}

// ============================================================================
// Process block
// ============================================================================
void TwelveOperatorSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    const int numSamples = buffer.getNumSamples();

    // Handle MIDI then render voices
    for (const auto metadata : midiMessages)
        handleMidiMessage(metadata.getMessage());

    for (auto& v : voices)
    {
        if (!v.active && !v.synth.isActive())
            continue;

        v.synth.render(buffer, 0, numSamples, v.frequency, 1.0f, true);
        v.ageCount += numSamples;

        if (!v.synth.isActive())
            v.active = false;
    }

    // Apply master FX chain
    const bool chorusOn  = apvts.getRawParameterValue(PARAM_CHORUS_ENABLE)->load() > 0.5f;
    const bool delayOn   = apvts.getRawParameterValue(PARAM_DELAY_ENABLE)->load()  > 0.5f;
    const bool reverbOn  = apvts.getRawParameterValue(PARAM_REVERB_ENABLE)->load() > 0.5f;
    const bool compOn    = apvts.getRawParameterValue(PARAM_COMP_ENABLE)->load()   > 0.5f;

    if (chorusOn) chorus.process(buffer);
    if (delayOn)  delay.process(buffer);
    if (reverbOn) reverb.process(buffer);
    if (compOn)   compressor.process(buffer);
}

// ============================================================================
// MIDI handling
// ============================================================================
void TwelveOperatorSynthProcessor::handleMidiMessage(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
        allocateVoice(msg.getNoteNumber(), msg.getVelocity() / 127.0f);
    else if (msg.isNoteOff())
        releaseVoice(msg.getNoteNumber());
    else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        for (auto& v : voices) { v.synth.noteOff(); v.active = false; v.midiNote = -1; }
}

void TwelveOperatorSynthProcessor::allocateVoice(int midiNote, float velocity)
{
    // Prefer a free voice
    for (auto& v : voices)
    {
        if (!v.active && !v.synth.isActive())
        {
            v.midiNote  = midiNote;
            v.frequency = midiNoteToFreq(midiNote);
            v.active    = true;
            v.ageCount  = 0;
            syncVoiceParameters();
            v.synth.noteOn(velocity);
            return;
        }
    }

    // Voice steal: take the oldest active voice
    SynthVoice* oldest = nullptr;
    for (auto& v : voices)
        if (oldest == nullptr || v.ageCount > oldest->ageCount)
            oldest = &v;

    if (oldest != nullptr)
    {
        oldest->midiNote  = midiNote;
        oldest->frequency = midiNoteToFreq(midiNote);
        oldest->active    = true;
        oldest->ageCount  = 0;
        syncVoiceParameters();
        oldest->synth.noteOn(velocity);
    }
}

void TwelveOperatorSynthProcessor::releaseVoice(int midiNote)
{
    for (auto& v : voices)
        if (v.midiNote == midiNote && v.active)
            v.synth.noteOff();
}

// ============================================================================
// Sync parameters to all voices
// ============================================================================
void TwelveOperatorSynthProcessor::syncVoiceParameters()
{
    const int   modeIdx  = static_cast<int>(apvts.getRawParameterValue(PARAM_MODE)->load());
    const int   algoIdx  = static_cast<int>(apvts.getRawParameterValue(PARAM_ALGORITHM)->load());
    const float gain     = apvts.getRawParameterValue(PARAM_MASTER_GAIN)->load();
    const float drift    = apvts.getRawParameterValue(PARAM_ANALOG_DRIFT)->load();

    const TwelveOperatorSynth::ADSRSettings adsr
    {
        apvts.getRawParameterValue(PARAM_ATTACK)->load(),
        apvts.getRawParameterValue(PARAM_DECAY)->load(),
        apvts.getRawParameterValue(PARAM_SUSTAIN)->load(),
        apvts.getRawParameterValue(PARAM_RELEASE)->load()
    };

    const TwelveOperatorSynth::VactrolSettings vactrol
    {
        apvts.getRawParameterValue(PARAM_VACTROL_ENABLE)->load()  > 0.5f,
        apvts.getRawParameterValue(PARAM_VACTROL_ATTACK)->load(),
        apvts.getRawParameterValue(PARAM_VACTROL_RELEASE)->load(),
        apvts.getRawParameterValue(PARAM_VACTROL_SAG)->load(),
        apvts.getRawParameterValue(PARAM_VACTROL_CURVE)->load()
    };

    const float hpCutoff = apvts.getRawParameterValue(PARAM_HP_CUTOFF)->load();
    const float hpQ      = apvts.getRawParameterValue(PARAM_HP_Q)->load();
    const float lpCutoff = apvts.getRawParameterValue(PARAM_LP_CUTOFF)->load();
    const float lpQ      = apvts.getRawParameterValue(PARAM_LP_Q)->load();

    for (auto& v : voices)
    {
        v.synth.setMode(static_cast<TwelveOperatorSynth::Mode>(modeIdx));
        v.synth.setAlgorithmPreset(static_cast<TwelveOperatorSynth::AlgorithmPreset>(algoIdx));
        v.synth.setMasterGain(gain);
        v.synth.setGlobalAnalogDrift(drift);
        v.synth.setGlobalADSR(adsr);
        v.synth.setVactrolSettings(vactrol);
        v.synth.setHighPassCutoff(hpCutoff);
        v.synth.setHighPassResonance(hpQ);
        v.synth.setLowPassCutoff(lpCutoff);
        v.synth.setLowPassResonance(lpQ);
        v.synth.setFiltersEnabled(true);
    }

    // Chorus
    chorus.setRate(apvts.getRawParameterValue(PARAM_CHORUS_RATE)->load());
    chorus.setDepth(apvts.getRawParameterValue(PARAM_CHORUS_DEPTH)->load());
    chorus.setMix(apvts.getRawParameterValue(PARAM_CHORUS_MIX)->load());

    // Delay
    delay.setDelayTime(apvts.getRawParameterValue(PARAM_DELAY_TIME)->load());
    delay.setFeedback(apvts.getRawParameterValue(PARAM_DELAY_FEEDBACK)->load());
    delay.setMix(apvts.getRawParameterValue(PARAM_DELAY_MIX)->load());

    // Reverb
    reverb.setRoomSize(apvts.getRawParameterValue(PARAM_REVERB_SIZE)->load());
    reverb.setDamping(apvts.getRawParameterValue(PARAM_REVERB_DAMP)->load());
    reverb.setWetLevel(apvts.getRawParameterValue(PARAM_REVERB_MIX)->load());
    reverb.setDryLevel(1.0f - apvts.getRawParameterValue(PARAM_REVERB_MIX)->load());

    // Compressor
    compressor.setThreshold(apvts.getRawParameterValue(PARAM_COMP_THRESHOLD)->load());
    compressor.setRatio(apvts.getRawParameterValue(PARAM_COMP_RATIO)->load());
}

void TwelveOperatorSynthProcessor::parameterChanged(const juce::String&, float)
{
    syncVoiceParameters();
}

// ============================================================================
// State
// ============================================================================
void TwelveOperatorSynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void TwelveOperatorSynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ============================================================================
// Helpers
// ============================================================================
float TwelveOperatorSynthProcessor::midiNoteToFreq(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

// ============================================================================
// Editor factory (defined in Editor file)
// ============================================================================
juce::AudioProcessorEditor* TwelveOperatorSynthProcessor::createEditor()
{
    return new TwelveOperatorSynthEditor(*this);
}

// ============================================================================
// Plugin entry point
// ============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TwelveOperatorSynthProcessor();
}
