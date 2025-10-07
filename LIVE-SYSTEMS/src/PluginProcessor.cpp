#include "../include/PluginProcessor.h"
#include "../include/PluginEditor.h"

//==============================================================================
LiveSystemsProcessor::LiveSystemsProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                     )
{
    // Initialize framework components
    parameterManager = std::make_unique<ParameterManager>(*this);
    audioEngine = std::make_unique<AudioEngine>();
    presetManager = std::make_unique<PresetManager>();
    
    // Note: Don't call initializeParameters() here as it's pure virtual
    // Derived classes must call it in their constructor after this constructor completes
}

LiveSystemsProcessor::~LiveSystemsProcessor()
{
}

//==============================================================================
const juce::String LiveSystemsProcessor::getName() const
{
    return getPluginName();
}

bool LiveSystemsProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LiveSystemsProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LiveSystemsProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LiveSystemsProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LiveSystemsProcessor::getNumPrograms()
{
    return presetManager->getNumPresets();
}

int LiveSystemsProcessor::getCurrentProgram()
{
    return presetManager->getCurrentPresetIndex();
}

void LiveSystemsProcessor::setCurrentProgram(int index)
{
    presetManager->loadPreset(index);
}

const juce::String LiveSystemsProcessor::getProgramName(int index)
{
    auto presets = presetManager->getAllPresets();
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[static_cast<size_t>(index)].name;
    return {};
}

void LiveSystemsProcessor::changeProgramName(int index, const juce::String& newName)
{
    // Implementation for renaming presets could go here
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void LiveSystemsProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    audioEngine->prepare(spec);
}

void LiveSystemsProcessor::releaseResources()
{
    audioEngine->reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LiveSystemsProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void LiveSystemsProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Process through the audio engine first (framework processing)
    audioEngine->process(buffer);
    
    // Then call the derived class's audio processing
    processAudio(buffer, midiMessages);
}

//==============================================================================
bool LiveSystemsProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LiveSystemsProcessor::createEditor()
{
    // This should be overridden by derived classes to return their specific editor
    return nullptr;
}

//==============================================================================
void LiveSystemsProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager->getStateInformation(destData);
}

void LiveSystemsProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager->setStateInformation(data, sizeInBytes);
}