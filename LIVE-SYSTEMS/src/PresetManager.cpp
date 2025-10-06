#include "../include/PresetManager.h"

//==============================================================================
PresetManager::PresetManager()
{
    createDefaultPresetDirectory();
    loadFactoryPresets();
    scanForPresets();
}

PresetManager::~PresetManager()
{
}

//==============================================================================
bool PresetManager::savePreset(const juce::String& name,
                                const juce::String& category,
                                const juce::ValueTree& state,
                                const juce::String& author,
                                const juce::String& description)
{
    Preset preset;
    preset.name = sanitizePresetName(name);
    preset.category = category;
    preset.author = author;
    preset.description = description;
    preset.state = state.createCopy();
    
    // Create file path
    auto categoryDir = presetDirectory.getChildFile(category);
    if (!categoryDir.exists())
        categoryDir.createDirectory();
    
    preset.file = categoryDir.getChildFile(preset.name + presetExtension);
    
    if (savePresetToFile(preset))
    {
        // Add to presets list (replace if exists)
        auto it = std::find_if(presets.begin(), presets.end(),
            [&](const Preset& p) { return p.name == preset.name && p.category == preset.category; });
        
        if (it != presets.end())
            *it = preset;
        else
            presets.push_back(preset);
        
        sortPresets();
        
        if (onPresetSaved)
            onPresetSaved(preset.name);
        
        if (onPresetListChanged)
            onPresetListChanged();
        
        return true;
    }
    
    return false;
}

bool PresetManager::loadPreset(const juce::String& name)
{
    auto it = std::find_if(presets.begin(), presets.end(),
        [&](const Preset& p) { return p.name == name; });
    
    if (it != presets.end())
    {
        currentPreset = *it;
        currentPresetIndex = static_cast<int>(std::distance(presets.begin(), it));
        
        if (onPresetLoaded)
            onPresetLoaded(currentPreset);
        
        return true;
    }
    
    return false;
}

bool PresetManager::loadPreset(int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
    {
        currentPreset = presets[static_cast<size_t>(index)];
        currentPresetIndex = index;
        
        if (onPresetLoaded)
            onPresetLoaded(currentPreset);
        
        return true;
    }
    
    return false;
}

bool PresetManager::deletePreset(const juce::String& name)
{
    auto it = std::find_if(presets.begin(), presets.end(),
        [&](const Preset& p) { return p.name == name; });
    
    if (it != presets.end())
    {
        if (it->file.exists())
            it->file.deleteFile();
        
        presets.erase(it);
        
        if (onPresetListChanged)
            onPresetListChanged();
        
        return true;
    }
    
    return false;
}

//==============================================================================
std::vector<PresetManager::Preset> PresetManager::getPresetsInCategory(const juce::String& category) const
{
    std::vector<Preset> categoryPresets;
    
    for (const auto& preset : presets)
    {
        if (preset.category == category)
            categoryPresets.push_back(preset);
    }
    
    return categoryPresets;
}

juce::StringArray PresetManager::getCategories() const
{
    juce::StringArray categories;
    
    for (const auto& preset : presets)
    {
        if (!categories.contains(preset.category))
            categories.add(preset.category);
    }
    
    categories.sort(false);
    return categories;
}

//==============================================================================
void PresetManager::addFactoryPreset(const juce::String& name,
                                      const juce::String& category,
                                      const juce::ValueTree& state,
                                      const juce::String& author,
                                      const juce::String& description)
{
    Preset preset;
    preset.name = name;
    preset.category = category;
    preset.author = author;
    preset.description = description;
    preset.state = state.createCopy();
    
    // Factory presets are stored in a special directory
    auto factoryDir = presetDirectory.getChildFile("Factory").getChildFile(category);
    if (!factoryDir.exists())
        factoryDir.createDirectory();
    
    preset.file = factoryDir.getChildFile(name + presetExtension);
    
    if (savePresetToFile(preset))
    {
        presets.push_back(preset);
        sortPresets();
    }
}

void PresetManager::loadFactoryPresets()
{
    // This would typically load built-in presets
    // For now, we'll create a simple default preset
    juce::ValueTree defaultState("DefaultPreset");
    defaultState.setProperty("name", "Default", nullptr);
    
    addFactoryPreset("Default", "Factory", defaultState, "LIVE-SYSTEMS", "Default factory preset");
}

//==============================================================================
void PresetManager::setPresetDirectory(const juce::File& directory)
{
    presetDirectory = directory;
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();
    
    scanForPresets();
}

void PresetManager::scanForPresets()
{
    presets.clear();
    
    if (!presetDirectory.exists())
        return;
    
    auto presetFiles = presetDirectory.findChildFiles(juce::File::findFiles, true, "*" + presetExtension);
    
    for (const auto& file : presetFiles)
    {
        if (isValidPresetFile(file))
        {
            auto preset = loadPresetFromFile(file);
            if (preset.isValid())
                presets.push_back(preset);
        }
    }
    
    sortPresets();
    
    if (onPresetListChanged)
        onPresetListChanged();
}

bool PresetManager::importPreset(const juce::File& presetFile)
{
    if (!isValidPresetFile(presetFile))
        return false;
    
    auto preset = loadPresetFromFile(presetFile);
    if (!preset.isValid())
        return false;
    
    // Copy to preset directory
    auto newFile = presetDirectory.getChildFile(preset.category).getChildFile(preset.name + presetExtension);
    if (presetFile.copyFileTo(newFile))
    {
        preset.file = newFile;
        presets.push_back(preset);
        sortPresets();
        
        if (onPresetListChanged)
            onPresetListChanged();
        
        return true;
    }
    
    return false;
}

bool PresetManager::exportPreset(const Preset& preset, const juce::File& destination)
{
    return preset.file.copyFileTo(destination);
}

//==============================================================================
juce::String PresetManager::sanitizePresetName(const juce::String& name)
{
    return name.removeCharacters("\\/:*?\"<>|").substring(0, 100);
}

bool PresetManager::isValidPresetFile(const juce::File& file)
{
    return file.hasFileExtension(".livepre") && file.getSize() > 0;
}

//==============================================================================
void PresetManager::createDefaultPresetDirectory()
{
    presetDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                          .getChildFile("LIVE-SYSTEMS")
                          .getChildFile("Presets");
    
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();
}

PresetManager::Preset PresetManager::loadPresetFromFile(const juce::File& file)
{
    Preset preset;
    preset.file = file;
    
    auto xml = juce::parseXML(file);
    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
        {
            preset.name = state.getProperty("name", file.getFileNameWithoutExtension());
            preset.category = state.getProperty("category", "User");
            preset.author = state.getProperty("author", "");
            preset.description = state.getProperty("description", "");
            preset.state = state;
        }
    }
    
    return preset;
}

bool PresetManager::savePresetToFile(const Preset& preset)
{
    auto state = preset.state.createCopy();
    state.setProperty("name", preset.name, nullptr);
    state.setProperty("category", preset.category, nullptr);
    state.setProperty("author", preset.author, nullptr);
    state.setProperty("description", preset.description, nullptr);
    
    auto xml = state.createXml();
    if (xml != nullptr)
    {
        return xml->writeTo(preset.file);
    }
    
    return false;
}

void PresetManager::sortPresets()
{
    std::sort(presets.begin(), presets.end(),
        [](const Preset& a, const Preset& b) {
            if (a.category != b.category)
                return a.category < b.category;
            return a.name < b.name;
        });
}