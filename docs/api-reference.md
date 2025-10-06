# API Reference

Complete reference for the LIVE-SYSTEMS framework classes and methods.

## Core Classes

### LiveSystemsProcessor

Base class for all LIVE-SYSTEMS plugins.

```cpp
class LiveSystemsProcessor : public juce::AudioProcessor
```

#### Constructor/Destructor

```cpp
LiveSystemsProcessor();
virtual ~LiveSystemsProcessor();
```

#### Pure Virtual Methods (Must Override)

```cpp
// Initialize plugin parameters
virtual void initializeParameters() = 0;

// Process audio samples
virtual void processAudio(juce::AudioBuffer<float>& buffer, 
                         juce::MidiBuffer& midiMessages) = 0;

// Return plugin name
virtual juce::String getPluginName() const = 0;
```

#### Virtual Methods (Optional Override)

```cpp
// Create custom editor (default returns nullptr)
virtual juce::AudioProcessorEditor* createEditor() override;
```

#### Protected Members

```cpp
std::unique_ptr<ParameterManager> parameterManager;
std::unique_ptr<AudioEngine> audioEngine;
std::unique_ptr<PresetManager> presetManager;
```

### LiveSystemsEditor

Base class for plugin editors with theming support.

```cpp
class LiveSystemsEditor : public juce::AudioProcessorEditor, private juce::Timer
```

#### Constructor/Destructor

```cpp
LiveSystemsEditor(LiveSystemsProcessor& processor);
virtual ~LiveSystemsEditor();
```

#### Pure Virtual Methods (Must Override)

```cpp
// Setup GUI components
virtual void setupComponents() = 0;

// Paint custom background
virtual void paintBackground(juce::Graphics& g) = 0;

// Layout components
virtual void layoutComponents() = 0;
```

#### Theme Structure

```cpp
struct Theme
{
    juce::Colour backgroundColour{ 0xff2d2d2d };
    juce::Colour primaryColour{ 0xff0080ff };
    juce::Colour secondaryColour{ 0xff404040 };
    juce::Colour textColour{ 0xffffffff };
    juce::Colour outlineColour{ 0xff606060 };
    
    juce::Font primaryFont{ 14.0f };
    juce::Font headerFont{ 18.0f, juce::Font::bold };
};
```

#### Protected Members

```cpp
Theme currentTheme;
LiveSystemsProcessor& audioProcessor;
```

## Parameter Management

### ParameterManager

Manages plugin parameters, state, and automation.

```cpp
class ParameterManager
```

#### Parameter Creation

```cpp
// Float parameter
juce::AudioParameterFloat* addFloatParameter(const ParameterInfo& info);

// Choice parameter
juce::AudioParameterChoice* addChoiceParameter(const juce::String& id,
                                               const juce::String& name,
                                               const juce::StringArray& choices,
                                               int defaultIndex);

// Boolean parameter
juce::AudioParameterBool* addBoolParameter(const juce::String& id,
                                           const juce::String& name,
                                           bool defaultValue);
```

#### Parameter Access

```cpp
// Get parameter object
juce::RangedAudioParameter* getParameter(const juce::String& id);

// Get parameter value
float getParameterValue(const juce::String& id);

// Set parameter value
void setParameterValue(const juce::String& id, float value);
```

#### Parameter Groups

```cpp
// Create parameter group
void createParameterGroup(const juce::String& groupName, 
                         const juce::StringArray& parameterIds);

// Get parameter group
juce::StringArray getParameterGroup(const juce::String& groupName);
```

#### Parameter Listeners

```cpp
// Add parameter listener
void addParameterListener(const juce::String& id, 
                         std::function<void(float)> callback);

// Remove parameter listener
void removeParameterListener(const juce::String& id);
```

#### State Management

```cpp
// Save state
void getStateInformation(juce::MemoryBlock& destData);

// Load state
void setStateInformation(const void* data, int sizeInBytes);

// Create parameter layout for AudioProcessor
juce::AudioProcessorParameterGroup createParameterLayout();
```

#### ParameterInfo Structure

```cpp
struct ParameterInfo
{
    juce::String id;                                        // Unique identifier
    juce::String name;                                      // Display name
    float defaultValue;                                     // Default value
    float minValue;                                         // Minimum value
    float maxValue;                                         // Maximum value
    juce::String units;                                     // Unit string
    std::function<juce::String(float)> valueToText;         // Value formatter
    std::function<float(const juce::String&)> textToValue;  // Text parser
};
```

## Audio Processing

### AudioEngine

High-performance audio processing with built-in effects.

```cpp
class AudioEngine
```

#### Initialization

```cpp
// Prepare for audio processing
void prepare(const juce::dsp::ProcessSpec& spec);

// Process audio buffer
void process(juce::AudioBuffer<float>& buffer);

// Reset processing state
void reset();
```

#### Effect Chain Management

```cpp
// Add custom processor to chain
template<typename ProcessorType>
void addProcessor(std::unique_ptr<ProcessorType> processor);

// Remove processor from chain
void removeProcessor(int index);

// Clear all processors
void clearProcessors();
```

#### Built-in Effects

```cpp
// Get filter processor
Filter& getFilter();

// Get compressor processor
Compressor& getCompressor();

// Get reverb processor
Reverb& getReverb();
```

#### Performance Monitoring

```cpp
// Get CPU usage percentage
double getCpuUsage() const;

// Update CPU usage (called internally)
void updateCpuUsage(double usage);
```

### AudioEngine::Filter

State variable filter with multiple types.

```cpp
class AudioEngine::Filter
```

#### Filter Types

```cpp
enum Type { LowPass, HighPass, BandPass, Notch };
```

#### Configuration

```cpp
// Set filter type
void setType(Type type);

// Set cutoff frequency (Hz)
void setFrequency(float frequency);

// Set resonance (0.0 to 1.0)
void setResonance(float resonance);
```

#### Processing

```cpp
// Prepare filter
void prepare(const juce::dsp::ProcessSpec& spec);

// Process audio
void process(juce::AudioBuffer<float>& buffer);

// Reset filter state
void reset();
```

### AudioEngine::Compressor

Dynamic range compressor.

```cpp
class AudioEngine::Compressor
```

#### Configuration

```cpp
// Set threshold in dB
void setThreshold(float thresholdDb);

// Set compression ratio
void setRatio(float ratio);

// Set attack time in milliseconds
void setAttack(float attackMs);

// Set release time in milliseconds
void setRelease(float releaseMs);
```

#### Processing

```cpp
void prepare(const juce::dsp::ProcessSpec& spec);
void process(juce::AudioBuffer<float>& buffer);
void reset();
```

### AudioEngine::Reverb

Algorithmic reverb processor.

```cpp
class AudioEngine::Reverb
```

#### Configuration

```cpp
// Set room size (0.0 to 1.0)
void setRoomSize(float size);

// Set damping (0.0 to 1.0)
void setDamping(float damping);

// Set wet level (0.0 to 1.0)
void setWetLevel(float wetLevel);

// Set dry level (0.0 to 1.0)
void setDryLevel(float dryLevel);
```

#### Processing

```cpp
void prepare(const juce::dsp::ProcessSpec& spec);
void process(juce::AudioBuffer<float>& buffer);
void reset();
```

## Preset Management

### PresetManager

Comprehensive preset management system.

```cpp
class PresetManager
```

#### Preset Structure

```cpp
struct Preset
{
    juce::String name;          // Preset name
    juce::String category;      // Category (e.g., "User", "Factory")
    juce::String author;        // Preset author
    juce::String description;   // Preset description
    juce::ValueTree state;     // Parameter state
    juce::File file;           // File location
    
    bool isValid() const;      // Check if preset is valid
};
```

#### Preset Operations

```cpp
// Save preset
bool savePreset(const juce::String& name, 
                const juce::String& category,
                const juce::ValueTree& state,
                const juce::String& author = {},
                const juce::String& description = {});

// Load preset by name
bool loadPreset(const juce::String& name);

// Load preset by index
bool loadPreset(int index);

// Delete preset
bool deletePreset(const juce::String& name);
```

#### Preset Navigation

```cpp
// Get number of presets
int getNumPresets() const;

// Get current preset
Preset getCurrentPreset() const;

// Get current preset index
int getCurrentPresetIndex() const;

// Get all presets
const std::vector<Preset>& getAllPresets() const;

// Get presets in category
std::vector<Preset> getPresetsInCategory(const juce::String& category) const;

// Get all categories
juce::StringArray getCategories() const;
```

#### Factory Presets

```cpp
// Add factory preset
void addFactoryPreset(const juce::String& name,
                      const juce::String& category,
                      const juce::ValueTree& state,
                      const juce::String& author = "LIVE-SYSTEMS",
                      const juce::String& description = {});

// Load all factory presets
void loadFactoryPresets();
```

#### File Management

```cpp
// Set preset directory
void setPresetDirectory(const juce::File& directory);

// Get preset directory
juce::File getPresetDirectory() const;

// Scan for presets in directory
void scanForPresets();

// Import preset from file
bool importPreset(const juce::File& presetFile);

// Export preset to file
bool exportPreset(const Preset& preset, const juce::File& destination);
```

#### Callbacks

```cpp
// Called when preset is loaded
std::function<void(const Preset&)> onPresetLoaded;

// Called when preset is saved
std::function<void(const juce::String&)> onPresetSaved;

// Called when preset list changes
std::function<void()> onPresetListChanged;
```

#### Utility Methods

```cpp
// Sanitize preset name for file system
static juce::String sanitizePresetName(const juce::String& name);

// Check if file is valid preset
static bool isValidPresetFile(const juce::File& file);
```

## Utility Functions

### Common DSP Utilities

```cpp
namespace LiveSystemsUtils
{
    // Convert dB to linear gain
    inline float dbToGain(float db) 
    { 
        return juce::Decibels::decibelsToGain(db); 
    }
    
    // Convert linear gain to dB
    inline float gainToDb(float gain) 
    { 
        return juce::Decibels::gainToDecibels(gain); 
    }
    
    // Linear interpolation
    template<typename T>
    inline T lerp(T a, T b, T t) 
    { 
        return a + t * (b - a); 
    }
    
    // Clamp value to range
    template<typename T>
    inline T clamp(T value, T min, T max) 
    { 
        return juce::jlimit(min, max, value); 
    }
}
```

### Parameter Helpers

```cpp
// Create standard gain parameter
inline ParameterManager::ParameterInfo createGainParameter(
    const juce::String& id = "gain",
    const juce::String& name = "Gain",
    float defaultDb = 0.0f,
    float minDb = -24.0f,
    float maxDb = 24.0f)
{
    ParameterManager::ParameterInfo info;
    info.id = id;
    info.name = name;
    info.defaultValue = defaultDb;
    info.minValue = minDb;
    info.maxValue = maxDb;
    info.units = "dB";
    info.valueToText = [](float value) { return juce::String(value, 1) + " dB"; };
    info.textToValue = [](const juce::String& text) { return text.getFloatValue(); };
    return info;
}

// Create standard frequency parameter
inline ParameterManager::ParameterInfo createFrequencyParameter(
    const juce::String& id = "frequency",
    const juce::String& name = "Frequency",
    float defaultHz = 1000.0f)
{
    ParameterManager::ParameterInfo info;
    info.id = id;
    info.name = name;
    info.defaultValue = defaultHz;
    info.minValue = 20.0f;
    info.maxValue = 20000.0f;
    info.units = "Hz";
    info.valueToText = [](float value) {
        if (value < 1000.0f)
            return juce::String(static_cast<int>(value)) + " Hz";
        return juce::String(value / 1000.0f, 1) + " kHz";
    };
    info.textToValue = [](const juce::String& text) {
        float value = text.getFloatValue();
        if (text.contains("kHz") || text.contains("k"))
            return value * 1000.0f;
        return value;
    };
    return info;
}
```

## Error Handling

### Exception Types

The framework uses standard C++ exceptions:

```cpp
// Parameter not found
std::invalid_argument("Parameter not found: " + id);

// Invalid parameter value
std::out_of_range("Parameter value out of range");

// File operation failed
std::runtime_error("Failed to save preset file");
```

### Safe Parameter Access

```cpp
// Always check parameter existence
auto* param = parameterManager->getParameter("gain");
if (param != nullptr)
{
    float value = param->getValue();
    // Use parameter safely
}
else
{
    // Handle missing parameter
    juce::Logger::writeToLog("Warning: Parameter 'gain' not found");
}
```

## Threading Considerations

### Audio Thread Safety

The following methods are safe to call from the audio thread:

```cpp
// Parameter access (read-only)
float value = parameterManager->getParameterValue("gain");

// Audio processing
audioEngine->process(buffer);

// Built-in effect processing
audioEngine->getFilter().process(buffer);
```

### Message Thread Only

The following methods must only be called from the message thread:

```cpp
// Parameter creation
parameterManager->addFloatParameter(info);

// Preset operations
presetManager->savePreset(name, category, state);

// File operations
presetManager->scanForPresets();

// GUI operations
addAndMakeVisible(component);
```

## Performance Guidelines

### Memory Allocation

- **Avoid allocations in audio thread**: Pre-allocate all buffers and objects
- **Use stack allocation**: Prefer stack objects over heap allocation
- **RAII pattern**: Use smart pointers for automatic cleanup

### Audio Processing

- **Process in blocks**: Avoid per-sample function calls
- **Cache parameter values**: Don't query parameters every sample
- **Use SIMD**: Leverage vectorized operations where possible

### GUI Updates

- **Limit update rate**: Use timers to control GUI refresh rate
- **Batch updates**: Group multiple GUI changes together
- **Avoid blocking**: Don't block the message thread with heavy operations

This API reference provides the complete interface for developing plugins with the LIVE-SYSTEMS framework.