#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class MidiChordSplitterAudioProcessor final : public juce::AudioProcessor
{
public:
    // Existing parameters for strummin



    // New parameters for high, mid, and low toggles and knobs
    juce::AudioParameterBool* highToggle;
    juce::AudioParameterFloat* highKnob;
    juce::AudioParameterBool* midToggle;
    juce::AudioParameterFloat* midKnob;
    juce::AudioParameterBool* lowToggle;
    juce::AudioParameterFloat* lowKnob;

    //==============================================================================
    MidiChordSplitterAudioProcessor();
    ~MidiChordSplitterAudioProcessor() override;
    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    std::vector<int> timeSigList =
    {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        256
    };
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;


    juce::ValueTree getParameters() { return parameters.copyState(); }

private:
    std::map<int, int> preholdNotes;
    juce::MidiBuffer preholdMidiBuffer;
    juce::AudioPlayHead* playHead{};
    juce::AudioPlayHead::CurrentPositionInfo cpi;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> * highToggleParameter = nullptr;
    std::atomic<float> * highKnobParameter = nullptr;
    std::atomic<float> * midToggleParameter = nullptr;
    std::atomic<float> * midKnobParameter = nullptr;
    std::atomic<float> * lowToggleParameter = nullptr;
    std::atomic<float> * lowKnobParameter = nullptr;

    // New auxiliary functions
    void processToggleNotes(const juce::SortedSet<int>& notes, juce::SortedSet<int>& notesToProcess, float knobValue, bool isHighToggle,bool isLowToggle);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiChordSplitterAudioProcessor)
};
