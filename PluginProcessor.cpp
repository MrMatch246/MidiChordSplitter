#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiChordSplitterAudioProcessor::MidiChordSplitterAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ), parameters(*this, nullptr, juce::Identifier("ValueTreeState"),
                    {
                        // Existing parameters for strumming

                        // New parameters for high, mid, and low toggles and knobs
                        std::make_unique<juce::AudioParameterBool>("highToggle", "High Toggle", true),
                        std::make_unique<juce::AudioParameterInt>("highKnob", "High Knob", 0, 16, 0),
                        std::make_unique<juce::AudioParameterBool>("midToggle", "Mid Toggle", true),
                        std::make_unique<juce::AudioParameterInt>("midKnob", "Mid Knob", 0 , 16 , 1 ),
                        std::make_unique<juce::AudioParameterBool>("lowToggle", "Low Toggle", true),
                        std::make_unique<juce::AudioParameterInt>("lowKnob", "Low Knob", 0 , 16 , 0 ),
                    }
          )
{
    // Initialize the new parameters
    highToggleParameter = parameters.getRawParameterValue("highToggle");
    highKnobParameter = parameters.getRawParameterValue("highKnob");
    midToggleParameter = parameters.getRawParameterValue("midToggle");
    midKnobParameter = parameters.getRawParameterValue("midKnob");
    lowToggleParameter = parameters.getRawParameterValue("lowToggle");
    lowKnobParameter = parameters.getRawParameterValue("lowKnob");
}

MidiChordSplitterAudioProcessor::~MidiChordSplitterAudioProcessor() {}

//==============================================================================
const juce::String MidiChordSplitterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiChordSplitterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MidiChordSplitterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MidiChordSplitterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MidiChordSplitterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiChordSplitterAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int MidiChordSplitterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiChordSplitterAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MidiChordSplitterAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MidiChordSplitterAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void MidiChordSplitterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MidiChordSplitterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool MidiChordSplitterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void MidiChordSplitterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();
    juce::MidiBuffer processedMidi;
    juce::SortedSet<int> notes;
    juce::SortedSet<int> notesToProcess;

    // Collect the notes in the chord
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            notes.add(msg.getNoteNumber());
        }
    }

    // Process notes based on the toggle and knob values
    if (highToggleParameter->load())
    {
        processToggleNotes(notes, notesToProcess, highKnobParameter->load(), true, false);
    }
    if (midToggleParameter->load())
    {
        processToggleNotes(notes, notesToProcess, midKnobParameter->load(), false, false);
    }
    if (lowToggleParameter->load())
    {
        processToggleNotes(notes, notesToProcess, lowKnobParameter->load(), false,true);
    }

    // Now process the MIDI messages with the filtered notes
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        const auto NoteNumber = msg.getNoteNumber();

        // Process "Note On" and "Note Off" events if the note is in the processed set
        if (notesToProcess.contains(NoteNumber))
        {
            processedMidi.addEvent(msg, metadata.samplePosition);
        }
        // Ensure "Note Off" events are also processed when the note is selected
        else if (msg.isNoteOff())
        {
            processedMidi.addEvent(msg, metadata.samplePosition);
        }
    }

    midiMessages.swapWith(processedMidi);
}

void MidiChordSplitterAudioProcessor::processToggleNotes(const juce::SortedSet<int>& notes, juce::SortedSet<int>& notesToProcess, float knobValue, bool isHighToggle, bool isLowToggle)
{
    if (notes.isEmpty()) return;

    const int numNotes = notes.size();
    int noteIndex = -1;

    // Handling High Toggle: Highest note based on knob value
    if (isHighToggle)
    {
        // For high toggle, select the highest note based on the knob value (0 = highest)
        noteIndex = numNotes - 1 - (int)knobValue; // Select based on descending order (highest note first)
    }
    // Handling Low Toggle: Lowest note based on knob value
    else if (isLowToggle)
    {
        // For low toggle, select the lowest notes based on knob value (0 = lowest)
        if (knobValue < numNotes)
        {
            noteIndex = (int)knobValue;
        }
    }
    // Handling Mid Toggle: Median note calculation (when knobValue >= 1)
    else if (!isHighToggle && !isLowToggle && knobValue >= 1.0f)
    {
        // For mid toggle, find the median
        int medianIndex = (numNotes - 1) / 2; // Default median (round down)
        if (knobValue > 1)
        {
            for (int i = medianIndex - knobValue/2; i <= medianIndex + knobValue/2; ++i)
            {
                if (i >= 0 && i < numNotes)
                {
                    notesToProcess.add(notes.getReference(i));
                }
            }
            return;
        }

        noteIndex = medianIndex;

    }

    // Ensure the knob value doesn't exceed available notes
    if (noteIndex >= 0 && noteIndex < numNotes)
    {
        // Add the note to the processing list
        notesToProcess.add(notes.getReference(noteIndex));
    }
}



bool MidiChordSplitterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiChordSplitterAudioProcessor::createEditor()
{
    return new MidiChordSplitterAudioProcessorEditor(*this, parameters);
}

void MidiChordSplitterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MidiChordSplitterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiChordSplitterAudioProcessor();
}
