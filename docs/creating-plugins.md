# Creating Plugins

This guide walks you through creating a complete VST plugin using the LIVE-SYSTEMS framework.

## Project Setup

### 1. Create Plugin Directory Structure

```bash
mkdir plugins/MyPlugin
cd plugins/MyPlugin
```

Your plugin directory should contain:
```
MyPlugin/
??? CMakeLists.txt
??? MyPluginProcessor.h
??? MyPluginProcessor.cpp
??? MyPluginEditor.h
??? MyPluginEditor.cpp
??? README.md (optional)
```

### 2. Configure CMakeLists.txt

Create a `CMakeLists.txt` file for your plugin:

```cmake
# Define your plugin
juce_add_plugin(MyPlugin
    COMPANY_NAME "Your Company Name"
    IS_SYNTH FALSE                    # TRUE for synthesizers
    NEEDS_MIDI_INPUT FALSE            # TRUE if plugin processes MIDI
    NEEDS_MIDI_OUTPUT FALSE           # TRUE if plugin generates MIDI
    IS_MIDI_EFFECT FALSE              # TRUE for MIDI-only effects
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE # TRUE if editor needs keyboard input
    COPY_PLUGIN_AFTER_BUILD TRUE      # Auto-install after build
    
    # Plugin identification (must be unique)
    PLUGIN_MANUFACTURER_CODE YrCo     # 4-char manufacturer code
    PLUGIN_CODE MyPl                  # 4-char plugin code
    
    # Supported formats
    FORMATS AU VST3 Standalone        # Add AAX if you have AAX SDK
    
    # Display name
    PRODUCT_NAME "My Plugin"
)

# Generate JUCE header
juce_generate_juce_header(MyPlugin)

# Add source files
target_sources(MyPlugin
    PRIVATE
        MyPluginProcessor.cpp
        MyPluginProcessor.h
        MyPluginEditor.cpp
        MyPluginEditor.h
)

# Link with framework and JUCE
target_link_libraries(MyPlugin
    PRIVATE
        LIVE-SYSTEMS-Framework      # Our framework
        juce::juce_audio_utils      # Additional JUCE modules if needed
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Set C++ standard
target_compile_features(MyPlugin PRIVATE cxx_std_20)

# Optional: Add compile definitions
target_compile_definitions(MyPlugin
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)
```

### 3. Register Plugin in Build System

Add your plugin to `plugins/CMakeLists.txt`:

```cmake
# Add your plugin here
add_subdirectory("MyPlugin")
```

## Implementing the Processor

### 1. Create Processor Header

```cpp
// MyPluginProcessor.h
#pragma once

#include "../../LIVE-SYSTEMS/include/PluginProcessor.h"

class MyPluginProcessor : public LiveSystemsProcessor
{
public:
    MyPluginProcessor();
    ~MyPluginProcessor() override;

    // Framework required overrides
    void initializeParameters() override;
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::String getPluginName() const override { return "My Plugin"; }
    juce::AudioProcessorEditor* createEditor() override;

private:
    // Parameter IDs (use consistent naming)
    static constexpr const char* GAIN_PARAM_ID = "gain";
    static constexpr const char* FREQUENCY_PARAM_ID = "frequency";
    static constexpr const char* RESONANCE_PARAM_ID = "resonance";

    // Plugin-specific members
    float currentGain = 1.0f;
    float currentFrequency = 1000.0f;
    float currentResonance = 0.7f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPluginProcessor)
};
```

### 2. Implement Processor Methods

```cpp
// MyPluginProcessor.cpp
#include "MyPluginProcessor.h"
#include "MyPluginEditor.h"

MyPluginProcessor::MyPluginProcessor()
{
    // Constructor - minimal initialization
}

MyPluginProcessor::~MyPluginProcessor()
{
    // Destructor - cleanup handled by base class
}

void MyPluginProcessor::initializeParameters()
{
    // Create gain parameter
    ParameterManager::ParameterInfo gainInfo;
    gainInfo.id = GAIN_PARAM_ID;
    gainInfo.name = "Gain";
    gainInfo.defaultValue = 0.0f;  // dB
    gainInfo.minValue = -24.0f;
    gainInfo.maxValue = 24.0f;
    gainInfo.units = "dB";
    gainInfo.valueToText = [](float value) { 
        return juce::String(value, 1) + " dB"; 
    };
    gainInfo.textToValue = [](const juce::String& text) { 
        return text.getFloatValue(); 
    };
    
    parameterManager->addFloatParameter(gainInfo);

    // Create frequency parameter
    ParameterManager::ParameterInfo freqInfo;
    freqInfo.id = FREQUENCY_PARAM_ID;
    freqInfo.name = "Frequency";
    freqInfo.defaultValue = 1000.0f;
    freqInfo.minValue = 20.0f;
    freqInfo.maxValue = 20000.0f;
    freqInfo.units = "Hz";
    freqInfo.valueToText = [](float value) {
        if (value < 1000.0f)
            return juce::String(static_cast<int>(value)) + " Hz";
        return juce::String(value / 1000.0f, 1) + " kHz";
    };
    
    parameterManager->addFloatParameter(freqInfo);

    // Create resonance parameter
    ParameterManager::ParameterInfo resInfo;
    resInfo.id = RESONANCE_PARAM_ID;
    resInfo.name = "Resonance";
    resInfo.defaultValue = 0.7f;
    resInfo.minValue = 0.1f;
    resInfo.maxValue = 1.0f;
    resInfo.units = "";
    
    parameterManager->addFloatParameter(resInfo);

    // Create parameter groups for organization
    parameterManager->createParameterGroup("Filter", 
        { FREQUENCY_PARAM_ID, RESONANCE_PARAM_ID });
    parameterManager->createParameterGroup("Output", 
        { GAIN_PARAM_ID });

    // Add parameter listeners for smooth updates
    parameterManager->addParameterListener(GAIN_PARAM_ID, 
        [this](float value) {
            currentGain = juce::Decibels::decibelsToGain(value);
        });
        
    parameterManager->addParameterListener(FREQUENCY_PARAM_ID,
        [this](float value) {
            currentFrequency = value;
            audioEngine->getFilter().setFrequency(value);
        });
        
    parameterManager->addParameterListener(RESONANCE_PARAM_ID,
        [this](float value) {
            currentResonance = value;
            audioEngine->getFilter().setResonance(value);
        });
}

void MyPluginProcessor::processAudio(juce::AudioBuffer<float>& buffer, 
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    // Update parameters if needed
    currentGain = juce::Decibels::decibelsToGain(
        parameterManager->getParameterValue(GAIN_PARAM_ID));
    
    // Apply filtering using the audio engine
    audioEngine->getFilter().setFrequency(
        parameterManager->getParameterValue(FREQUENCY_PARAM_ID));
    audioEngine->getFilter().setResonance(
        parameterManager->getParameterValue(RESONANCE_PARAM_ID));
    audioEngine->getFilter().process(buffer);

    // Apply gain
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= currentGain;
        }
    }
}

juce::AudioProcessorEditor* MyPluginProcessor::createEditor()
{
    return new MyPluginEditor(*this);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyPluginProcessor();
}
```

## Implementing the Editor

### 1. Create Editor Header

```cpp
// MyPluginEditor.h
#pragma once

#include "../../LIVE-SYSTEMS/include/PluginEditor.h"
#include "MyPluginProcessor.h"

class MyPluginEditor : public LiveSystemsEditor
{
public:
    MyPluginEditor(MyPluginProcessor&);
    ~MyPluginEditor() override;

protected:
    // Framework required overrides
    void setupComponents() override;
    void paintBackground(juce::Graphics& g) override;
    void layoutComponents() override;

private:
    // GUI Components
    juce::Slider gainSlider;
    juce::Slider frequencySlider;
    juce::Slider resonanceSlider;
    
    juce::Label gainLabel;
    juce::Label frequencyLabel;
    juce::Label resonanceLabel;
    juce::Label titleLabel;

    // Helper methods
    void setupSlider(juce::Slider& slider, const juce::String& paramId);
    void setupLabel(juce::Label& label, const juce::String& text);
    
    MyPluginProcessor& getProcessor() { 
        return static_cast<MyPluginProcessor&>(audioProcessor); 
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPluginEditor)
};
```

### 2. Implement Editor Methods

```cpp
// MyPluginEditor.cpp
#include "MyPluginEditor.h"

MyPluginEditor::MyPluginEditor(MyPluginProcessor& p)
    : LiveSystemsEditor(p)
{
    setSize(400, 300);
}

MyPluginEditor::~MyPluginEditor()
{
}

void MyPluginEditor::setupComponents()
{
    // Title
    titleLabel.setText("My Plugin", juce::dontSendNotification);
    titleLabel.setFont(currentTheme.headerFont);
    titleLabel.setColour(juce::Label::textColourId, currentTheme.textColour);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Setup sliders and labels
    setupSlider(gainSlider, "gain");
    setupLabel(gainLabel, "Gain");
    
    setupSlider(frequencySlider, "frequency");
    frequencySlider.setSkewFactorFromMidPoint(1000.0f); // Logarithmic scaling
    setupLabel(frequencyLabel, "Frequency");
    
    setupSlider(resonanceSlider, "resonance");
    setupLabel(resonanceLabel, "Resonance");
}

void MyPluginEditor::setupSlider(juce::Slider& slider, const juce::String& paramId)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    // Get parameter info from manager
    auto* param = getProcessor().parameterManager->getParameter(paramId);
    if (param != nullptr)
    {
        slider.setRange(param->getNormalisableRange().start, 
                       param->getNormalisableRange().end);
        slider.setValue(param->getValue());
        
        // Connect slider to parameter
        slider.onValueChange = [this, paramId, &slider]() {
            getProcessor().parameterManager->setParameterValue(paramId, 
                static_cast<float>(slider.getValue()));
        };
    }
    
    // Apply theme
    slider.setColour(juce::Slider::rotarySliderFillColourId, currentTheme.primaryColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, currentTheme.outlineColour);
    slider.setColour(juce::Slider::textBoxTextColourId, currentTheme.textColour);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, currentTheme.secondaryColour);
    
    addAndMakeVisible(slider);
}

void MyPluginEditor::setupLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(currentTheme.primaryFont);
    label.setColour(juce::Label::textColourId, currentTheme.textColour);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void MyPluginEditor::paintBackground(juce::Graphics& g)
{
    // Gradient background
    juce::ColourGradient gradient(
        currentTheme.backgroundColour.brighter(0.1f), 0, 0,
        currentTheme.backgroundColour.darker(0.2f), 0, static_cast<float>(getHeight()),
        false
    );
    
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    // Border
    g.setColour(currentTheme.outlineColour);
    g.drawRect(getLocalBounds(), 2);

    // Decorative panel
    g.setColour(currentTheme.primaryColour.withAlpha(0.1f));
    g.fillRoundedRectangle(20, 60, getWidth() - 40, getHeight() - 80, 10);
}

void MyPluginEditor::layoutComponents()
{
    auto bounds = getLocalBounds();
    
    // Title area
    titleLabel.setBounds(bounds.removeFromTop(50).reduced(10));
    
    // Control area
    auto controlArea = bounds.reduced(30);
    auto controlWidth = controlArea.getWidth() / 3;
    
    // Gain section
    auto gainArea = controlArea.removeFromLeft(controlWidth);
    gainLabel.setBounds(gainArea.removeFromTop(25));
    gainSlider.setBounds(gainArea.reduced(10));
    
    // Frequency section
    auto freqArea = controlArea.removeFromLeft(controlWidth);
    frequencyLabel.setBounds(freqArea.removeFromTop(25));
    frequencySlider.setBounds(freqArea.reduced(10));
    
    // Resonance section
    resonanceLabel.setBounds(controlArea.removeFromTop(25));
    resonanceSlider.setBounds(controlArea.reduced(10));
}
```

## Building and Testing

### 1. Build Your Plugin

```bash
cd build
cmake --build . --target MyPlugin
```

### 2. Test in Standalone Mode

The standalone version will be built automatically. Run it to test your plugin.

### 3. Install Plugin

```bash
cmake --build . --target MyPlugin --config Release
```

The plugin will be installed to the standard plugin directories:
- **Windows**: `%PROGRAMFILES%\Common Files\VST3\`
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/`
- **Linux**: `~/.vst3/`

## Advanced Features

### 1. Adding Custom DSP

```cpp
class MyCustomProcessor : public AudioEngine::ProcessorChain
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override
    {
        // Initialize your DSP
    }
    
    void process(juce::AudioBuffer<float>& buffer) override
    {
        // Custom audio processing
    }
    
    void reset() override
    {
        // Reset state
    }
};

// In your processor constructor:
audioEngine->addProcessor(std::make_unique<MyCustomProcessor>());
```

### 2. Factory Presets

```cpp
void MyPluginProcessor::initializeParameters()
{
    // ... parameter setup ...
    
    // Add factory presets
    juce::ValueTree cleanPreset("CleanPreset");
    cleanPreset.setProperty("gain", 0.0f, nullptr);
    cleanPreset.setProperty("frequency", 1000.0f, nullptr);
    cleanPreset.setProperty("resonance", 0.5f, nullptr);
    
    presetManager->addFactoryPreset("Clean", "Factory", cleanPreset);
    
    juce::ValueTree brightPreset("BrightPreset");
    brightPreset.setProperty("gain", 3.0f, nullptr);
    brightPreset.setProperty("frequency", 5000.0f, nullptr);
    brightPreset.setProperty("resonance", 0.8f, nullptr);
    
    presetManager->addFactoryPreset("Bright", "Factory", brightPreset);
}
```

### 3. Custom Parameter Types

```cpp
// Create choice parameter for filter type
juce::StringArray filterTypes = { "Low Pass", "High Pass", "Band Pass" };
parameterManager->addChoiceParameter("filterType", "Filter Type", filterTypes, 0);

// Create boolean parameter for bypass
parameterManager->addBoolParameter("bypass", "Bypass", false);
```

## Troubleshooting

### Common Issues

1. **Plugin not loading**: Check plugin format compatibility
2. **Audio glitches**: Ensure no allocations in `processAudio()`
3. **GUI not updating**: Verify parameter connections
4. **Build errors**: Check CMake configuration and dependencies

### Debugging Tips

1. Use the standalone version for easier debugging
2. Add logging to track parameter changes
3. Use JUCE's built-in debugging tools
4. Profile audio performance regularly

## Next Steps

- Read the [Parameter Management](parameter-management.md) guide
- Learn about [Audio Processing](audio-processing.md) techniques
- Explore [GUI Development](gui-development.md) options
- Study the example plugins for inspiration