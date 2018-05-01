/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include<iostream>

using namespace std;
//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), false)
                     #endif
                       ),
                        m_pCSin(0), parameters(*this, nullptr)
#endif
{
    CSinusoid::create(m_pCSin);
    NormalisableRange<float> frequencyRange(-100.0f, 0.0f);
    parameters.createAndAddParameter("frequencySliderID", "frequencySlider", "frequencySlider", frequencyRange, -80.0f, nullptr, nullptr);
    
    NormalisableRange<float> widthRange(-1200.0f, 1200.0f, 6);
    parameters.createAndAddParameter("widthSliderID", "widthSlider", "widthSlider", widthRange, 0, nullptr, nullptr);
    
    NormalisableRange<float> SineRange(1, 512, 1);
    parameters.createAndAddParameter("SineSliderID", "SineSlider", "SineSlider", SineRange, 512, nullptr, nullptr);
    
    parameters.state = ValueTree("savedParams");
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
    CSinusoid::destroy(m_pCSin);
    m_pCSin = 0;
    delete m_pfInputBuffer;
    delete m_pfOutputBuffer;
    delete m_pfOldBuffer;
}

//==============================================================================
const String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    m_pCSin->init(2*samplesPerBlock, samplesPerBlock, sampleRate, samplesPerBlock, 1, -80);
    m_pfInputBuffer = new float [2*samplesPerBlock]();
    m_pfOutputBuffer = new float [2*samplesPerBlock]();
    m_pfOldBuffer = new float [samplesPerBlock]();
    m_pfTestSine = new float [10000];
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    auto* channelData = buffer.getWritePointer(0);
    int iNumFrames = buffer.getNumSamples();
    
    for(int i = 0;i<iNumFrames;i++)
    {
        m_pfInputBuffer[i+iNumFrames] = channelData[i];
    }
    
    m_pCSin->analyze(m_pfInputBuffer);
    m_pCSin->synthesize(m_pfOutputBuffer);
    
    for(int i = 0;i<iNumFrames;i++)
    {
        channelData[i] = m_pfOutputBuffer[i]+m_pfOldBuffer[i];
        m_pfOldBuffer[i] = m_pfOutputBuffer[i+iNumFrames];
        m_pfInputBuffer[i] = m_pfInputBuffer[i+iNumFrames];
        cout<<channelData[i]<<endl;
    }
}
void NewProjectAudioProcessor::setSinusoidParameter (CSinusoid::SinusoidParam_t eParam, float fParamValue)
{
    m_pCSin->setParam(eParam, fParamValue);
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ScopedPointer<XmlElement> savedParameters (getXmlFromBinary(data, sizeInBytes));
    
    if(savedParameters != nullptr)
    {
        if(savedParameters->hasTagName(parameters.state.getType()))
        {
            parameters.state = ValueTree::fromXml(*savedParameters);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
