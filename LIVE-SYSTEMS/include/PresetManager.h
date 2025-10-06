#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <memory>
#include <vector>

//==============================================================================
/**
    Advanced preset management system for LIVE-SYSTEMS plugins.
    Supports preset saving/loading, categorization, and user preset management.
*/
class PresetManager
{
public:
    //==============================================================================
    struct Preset
    {
        juce::String name;
        juce::String category;
        juce::String author;
        juce::String description;
        juce::ValueTree state;
        juce::File file;
        
        bool isValid() const { return name.isNotEmpty() && state.isValid(); }
    };

    //==============================================================================
    PresetManager();
    ~PresetManager();

    //==============================================================================
    // Preset management
    bool savePreset(const juce::String& name, 
                    const juce::String& category,
                    const juce::ValueTree& state,
                    const juce::String& author = {},
                    const juce::String& description = {});
    
    bool loadPreset(const juce::String& name);
    bool loadPreset(int index);
    bool deletePreset(const juce::String& name);
    
    //==============================================================================
    // Preset navigation
    int getNumPresets() const { return static_cast<int>(presets.size()); }
    Preset getCurrentPreset() const { return currentPreset; }
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    
    const std::vector<Preset>& getAllPresets() const { return presets; }
    std::vector<Preset> getPresetsInCategory(const juce::String& category) const;
    juce::StringArray getCategories() const;
    
    //==============================================================================
    // Factory presets
    void addFactoryPreset(const juce::String& name,
                          const juce::String& category,
                          const juce::ValueTree& state,
                          const juce::String& author = "LIVE-SYSTEMS",
                          const juce::String& description = {});
    
    void loadFactoryPresets();
    
    //==============================================================================
    // File management
    void setPresetDirectory(const juce::File& directory);
    juce::File getPresetDirectory() const { return presetDirectory; }
    
    void scanForPresets();
    bool importPreset(const juce::File& presetFile);
    bool exportPreset(const Preset& preset, const juce::File& destination);
    
    //==============================================================================
    // Callbacks for state changes
    std::function<void(const Preset&)> onPresetLoaded;
    std::function<void(const juce::String&)> onPresetSaved;
    std::function<void()> onPresetListChanged;

    //==============================================================================
    // Utility
    static juce::String sanitizePresetName(const juce::String& name);
    static bool isValidPresetFile(const juce::File& file);

private:
    //==============================================================================
    std::vector<Preset> presets;
    Preset currentPreset;
    int currentPresetIndex = -1;
    
    juce::File presetDirectory;
    juce::String presetExtension = ".livepre";
    
    //==============================================================================
    // Internal methods
    void createDefaultPresetDirectory();
    Preset loadPresetFromFile(const juce::File& file);
    bool savePresetToFile(const Preset& preset);
    void sortPresets();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};