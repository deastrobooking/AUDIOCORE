# Parameter Management

The LIVE-SYSTEMS framework provides a comprehensive parameter management system that handles parameter creation, automation, state management, and GUI binding automatically.

## Overview

The `ParameterManager` class simplifies plugin parameter handling by providing:

- **Type-safe parameter creation** (Float, Choice, Boolean)
- **Automatic state serialization** and deserialization
- **Parameter grouping** for logical organization
- **Listener system** for real-time updates
- **Value formatting** for display purposes

## Parameter Types

### Float Parameters

Float parameters are the most common type for continuous controls:

```cpp
void MyPlugin::initializeParameters()
{
    ParameterManager::ParameterInfo gainInfo;
    gainInfo.id = "gain";                    // Unique identifier
    gainInfo.name = "Gain";                  // Display name
    gainInfo.defaultValue = 0.0f;            // Default value
    gainInfo.minValue = -24.0f;              // Minimum value
    gainInfo.maxValue = 24.0f;               // Maximum value
    gainInfo.units = "dB";                   // Unit string
    
    // Custom value formatting
    gainInfo.valueToText = [](float value) { 
        return juce::String(value, 1) + " dB"; 
    };
    
    // Custom text parsing
    gainInfo.textToValue = [](const juce::String& text) { 
        return text.getFloatValue(); 
    };
    
    parameterManager->addFloatParameter(gainInfo);
}
```

### Choice Parameters

For discrete options like filter types or waveforms:

```cpp
juce::StringArray filterTypes = { "Low Pass", "High Pass", "Band Pass", "Notch" };
auto* filterTypeParam = parameterManager->addChoiceParameter(
    "filterType",           // ID
    "Filter Type",          // Name
    filterTypes,            // Options
    0                       // Default index
);
```

### Boolean Parameters

For on/off switches:

```cpp
auto* bypassParam = parameterManager->addBoolParameter(
    "bypass",               // ID
    "Bypass",               // Name
    false                   // Default value
);
```

## Advanced Parameter Configuration

### Logarithmic Scaling

For frequency parameters that need logarithmic response:

```cpp
ParameterManager::ParameterInfo freqInfo;
freqInfo.id = "frequency";
freqInfo.name = "Frequency";
freqInfo.defaultValue = 1000.0f;
freqInfo.minValue = 20.0f;
freqInfo.maxValue = 20000.0f;
freqInfo.units = "Hz";

// Custom formatting for frequency display
freqInfo.valueToText = [](float value) {
    if (value < 1000.0f)
        return juce::String(static_cast<int>(value)) + " Hz";
    return juce::String(value / 1000.0f, 1) + " kHz";
};

freqInfo.textToValue = [](const juce::String& text) {
    auto numericPart = text.getFloatValue();
    if (text.contains("kHz") || text.contains("k"))
        return numericPart * 1000.0f;
    return numericPart;
};

parameterManager->addFloatParameter(freqInfo);
```

### Time-Based Parameters

For delay times, reverb decay, etc.:

```cpp
ParameterManager::ParameterInfo delayInfo;
delayInfo.id = "delayTime";
delayInfo.name = "Delay Time";
delayInfo.defaultValue = 250.0f;  // milliseconds
delayInfo.minValue = 1.0f;
delayInfo.maxValue = 2000.0f;
delayInfo.units = "ms";

delayInfo.valueToText = [](float value) {
    if (value < 1000.0f)
        return juce::String(static_cast<int>(value)) + " ms";
    return juce::String(value / 1000.0f, 2) + " s";
};

parameterManager->addFloatParameter(delayInfo);
```

### Percentage Parameters

For mix, drive, and other 0-100% controls:

```cpp
ParameterManager::ParameterInfo mixInfo;
mixInfo.id = "mix";
mixInfo.name = "Mix";
mixInfo.defaultValue = 0.5f;     // 0.0 to 1.0 internally
mixInfo.minValue = 0.0f;
mixInfo.maxValue = 1.0f;
mixInfo.units = "%";

mixInfo.valueToText = [](float value) {
    return juce::String(static_cast<int>(value * 100)) + "%";
};

mixInfo.textToValue = [](const juce::String& text) {
    return text.getFloatValue() / 100.0f;
};

parameterManager->addFloatParameter(mixInfo);
```

## Parameter Access

### Getting Parameter Values

```cpp
// In processAudio() or other methods
float gain = parameterManager->getParameterValue("gain");
float frequency = parameterManager->getParameterValue("frequency");
bool bypassed = parameterManager->getParameterValue("bypass") > 0.5f;

// Convert to usable values
float gainLinear = juce::Decibels::decibelsToGain(gain);
```

### Setting Parameter Values

```cpp
// Programmatically set parameter values
parameterManager->setParameterValue("gain", -6.0f);
parameterManager->setParameterValue("frequency", 2000.0f);
```

### Getting Parameter Objects

```cpp
// Access the parameter object directly
auto* gainParam = parameterManager->getParameter("gain");
if (gainParam != nullptr)
{
    float normalizedValue = gainParam->getValue();          // 0.0 to 1.0
    float actualValue = gainParam->getCurrentValue();       // Actual parameter value
    juce::String text = gainParam->getCurrentValueAsText(); // Formatted text
}
```

## Parameter Listeners

Parameter listeners allow real-time response to parameter changes:

### Basic Listener

```cpp
void MyPlugin::initializeParameters()
{
    // ... parameter creation ...
    
    // Add listener for gain parameter
    parameterManager->addParameterListener("gain", [this](float value) {
        // Convert to linear gain and store for audio processing
        currentGain = juce::Decibels::decibelsToGain(value);
    });
    
    // Add listener for filter frequency
    parameterManager->addParameterListener("frequency", [this](float value) {
        // Update filter immediately
        audioEngine->getFilter().setFrequency(value);
    });
}
```

### Advanced Listener with Smoothing

```cpp
class MyPlugin : public LiveSystemsProcessor
{
private:
    juce::LinearSmoothedValue<float> smoothedGain;

public:
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        LiveSystemsProcessor::prepareToPlay(sampleRate, samplesPerBlock);
        
        // Setup parameter smoothing
        smoothedGain.reset(sampleRate, 0.05); // 50ms ramp time
    }
    
    void initializeParameters() override
    {
        // ... parameter creation ...
        
        parameterManager->addParameterListener("gain", [this](float value) {
            float linearGain = juce::Decibels::decibelsToGain(value);
            smoothedGain.setTargetValue(linearGain);
        });
    }
    
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        // Use smoothed parameter values
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float gain = smoothedGain.getNextValue();
            
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* data = buffer.getWritePointer(channel);
                data[sample] *= gain;
            }
        }
    }
};
```

## Parameter Groups

Organize related parameters into logical groups:

```cpp
void MyPlugin::initializeParameters()
{
    // Create parameters...
    
    // Group filter-related parameters
    parameterManager->createParameterGroup("Filter", {
        "filterType",
        "frequency",
        "resonance",
        "filterBypass"
    });
    
    // Group output-related parameters
    parameterManager->createParameterGroup("Output", {
        "gain",
        "mix",
        "outputBypass"
    });
    
    // Group modulation parameters
    parameterManager->createParameterGroup("Modulation", {
        "lfoRate",
        "lfoDepth",
        "lfoShape"
    });
}

// Access parameter groups
juce::StringArray filterParams = parameterManager->getParameterGroup("Filter");
for (const auto& paramId : filterParams)
{
    auto* param = parameterManager->getParameter(paramId);
    // Process each parameter in the group
}
```

## State Management

The framework automatically handles parameter state saving and loading:

### Automatic State Management

```cpp
// Framework handles these automatically:
void getStateInformation(juce::MemoryBlock& destData) override
{
    parameterManager->getStateInformation(destData);
}

void setStateInformation(const void* data, int sizeInBytes) override
{
    parameterManager->setStateInformation(data, sizeInBytes);
}
```

### Custom State Data

Add custom data to the state:

```cpp
void MyPlugin::getStateInformation(juce::MemoryBlock& destData)
{
    // Get parameter state
    juce::MemoryBlock parameterData;
    parameterManager->getStateInformation(parameterData);
    
    // Create combined state
    juce::ValueTree state("PluginState");
    state.setProperty("parameters", parameterData.toBase64Encoding(), nullptr);
    state.setProperty("customData", myCustomValue, nullptr);
    state.setProperty("version", "1.0", nullptr);
    
    // Serialize to XML
    auto xml = state.createXml();
    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);
}

void MyPlugin::setStateInformation(const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
        {
            // Restore parameters
            if (state.hasProperty("parameters"))
            {
                juce::String paramData = state.getProperty("parameters");
                juce::MemoryBlock parameterData;
                parameterData.fromBase64Encoding(paramData);
                parameterManager->setStateInformation(parameterData.getData(), 
                                                     static_cast<int>(parameterData.getSize()));
            }
            
            // Restore custom data
            if (state.hasProperty("customData"))
                myCustomValue = state.getProperty("customData");
        }
    }
}
```

## GUI Integration

### Connecting Sliders to Parameters

```cpp
void MyPluginEditor::setupComponents()
{
    // Setup slider
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    
    // Get parameter and setup range
    auto* gainParam = getProcessor().parameterManager->getParameter("gain");
    if (gainParam != nullptr)
    {
        auto range = gainParam->getNormalisableRange();
        gainSlider.setRange(range.start, range.end);
        gainSlider.setValue(gainParam->getCurrentValue());
        
        // Connect slider to parameter
        gainSlider.onValueChange = [this, gainParam]() {
            gainParam->setValueNotifyingHost(
                gainParam->getNormalisableRange().convertTo0to1(
                    static_cast<float>(gainSlider.getValue())
                )
            );
        };
        
        // Update slider when parameter changes (automation)
        gainSlider.onDragStart = [gainParam]() {
            gainParam->beginChangeGesture();
        };
        
        gainSlider.onDragEnd = [gainParam]() {
            gainParam->endChangeGesture();
        };
    }
    
    addAndMakeVisible(gainSlider);
}
```

### Parameter Value Display

```cpp
class ParameterDisplay : public juce::Component, private juce::Timer
{
public:
    ParameterDisplay(ParameterManager& pm, const juce::String& paramId)
        : parameterManager(pm), parameterId(paramId)
    {
        startTimerHz(30); // Update 30 times per second
    }
    
    void paint(juce::Graphics& g) override
    {
        auto* param = parameterManager.getParameter(parameterId);
        if (param != nullptr)
        {
            g.setColour(juce::Colours::white);
            g.drawText(param->getCurrentValueAsText(), 
                      getLocalBounds(), 
                      juce::Justification::centred);
        }
    }
    
private:
    void timerCallback() override
    {
        repaint();
    }
    
    ParameterManager& parameterManager;
    juce::String parameterId;
};
```

## Best Practices

### Parameter Naming

```cpp
// Use consistent naming conventions
static constexpr const char* GAIN_PARAM_ID = "gain";
static constexpr const char* FREQUENCY_PARAM_ID = "frequency";
static constexpr const char* RESONANCE_PARAM_ID = "resonance";

// Group-based naming for complex plugins
static constexpr const char* FILTER1_FREQ_ID = "filter1_frequency";
static constexpr const char* FILTER2_FREQ_ID = "filter2_frequency";
static constexpr const char* LFO1_RATE_ID = "lfo1_rate";
```

### Performance Optimization

```cpp
// Cache frequently accessed parameters
class MyPlugin : public LiveSystemsProcessor
{
private:
    float cachedGain = 1.0f;
    float cachedFrequency = 1000.0f;
    
public:
    void initializeParameters() override
    {
        // ... parameter creation ...
        
        // Update cached values when parameters change
        parameterManager->addParameterListener("gain", [this](float value) {
            cachedGain = juce::Decibels::decibelsToGain(value);
        });
        
        parameterManager->addParameterListener("frequency", [this](float value) {
            cachedFrequency = value;
        });
    }
    
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        // Use cached values instead of querying parameter manager
        // This is more efficient in the audio thread
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                data[sample] *= cachedGain;
            }
        }
    }
};
```

### Parameter Validation

```cpp
ParameterManager::ParameterInfo freqInfo;
freqInfo.id = "frequency";
freqInfo.name = "Frequency";
freqInfo.defaultValue = 1000.0f;
freqInfo.minValue = 20.0f;
freqInfo.maxValue = 20000.0f;

// Add validation in textToValue function
freqInfo.textToValue = [](const juce::String& text) {
    float value = text.getFloatValue();
    
    // Clamp to valid range
    return juce::jlimit(20.0f, 20000.0f, value);
};
```

## Common Patterns

### ADSR Envelope Parameters

```cpp
void setupADSRParameters()
{
    // Attack
    ParameterManager::ParameterInfo attackInfo;
    attackInfo.id = "attack";
    attackInfo.name = "Attack";
    attackInfo.defaultValue = 10.0f;   // ms
    attackInfo.minValue = 0.1f;
    attackInfo.maxValue = 5000.0f;
    attackInfo.valueToText = [](float value) {
        return juce::String(static_cast<int>(value)) + " ms";
    };
    parameterManager->addFloatParameter(attackInfo);
    
    // Decay, Sustain, Release...
    // Similar pattern for each parameter
    
    // Group them together
    parameterManager->createParameterGroup("ADSR", {
        "attack", "decay", "sustain", "release"
    });
}
```

### Multi-band Parameters

```cpp
void setupMultibandParameters()
{
    for (int band = 1; band <= 3; ++band)
    {
        juce::String bandStr = juce::String(band);
        
        // Frequency
        ParameterManager::ParameterInfo freqInfo;
        freqInfo.id = "band" + bandStr + "_freq";
        freqInfo.name = "Band " + bandStr + " Freq";
        freqInfo.defaultValue = 1000.0f * band;
        freqInfo.minValue = 20.0f;
        freqInfo.maxValue = 20000.0f;
        parameterManager->addFloatParameter(freqInfo);
        
        // Gain
        ParameterManager::ParameterInfo gainInfo;
        gainInfo.id = "band" + bandStr + "_gain";
        gainInfo.name = "Band " + bandStr + " Gain";
        gainInfo.defaultValue = 0.0f;
        gainInfo.minValue = -20.0f;
        gainInfo.maxValue = 20.0f;
        parameterManager->addFloatParameter(gainInfo);
    }
}
```

This comprehensive parameter management system provides everything needed for professional plugin development while maintaining simplicity and performance.