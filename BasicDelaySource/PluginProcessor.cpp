/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
CircularDelayBufferAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayTime",
        "Delay Time",
        juce::NormalisableRange<float>(0.01f, 2.0f, 0.01f),
        0.5f));

    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "wetDry",
            "Wet/Dry",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.5f));

        return { params.begin(), params.end() };
}

CircularDelayBufferAudioProcessor::CircularDelayBufferAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
    #if ! JucePlugin_IsMidiEffect
     #if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
     #endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
    ),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

CircularDelayBufferAudioProcessor::~CircularDelayBufferAudioProcessor()
{
}



//==============================================================================
const juce::String CircularDelayBufferAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CircularDelayBufferAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CircularDelayBufferAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CircularDelayBufferAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CircularDelayBufferAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CircularDelayBufferAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CircularDelayBufferAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CircularDelayBufferAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CircularDelayBufferAudioProcessor::getProgramName (int index)
{
    return {};
}

void CircularDelayBufferAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CircularDelayBufferAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * 2.0;
    delayBuffer.setSize(getTotalNumOutputChannels(), (int)delayBufferSize);
}

void CircularDelayBufferAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CircularDelayBufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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
#endif

void CircularDelayBufferAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto wetDry = apvts.getRawParameterValue("wetDry")->load();
    auto dryGain = 1.0f - wetDry;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), dryGain);
    }

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        
        //copy input to delay buffer
        fillBuffer(buffer, channel);
        //read from past in delay then add it back to the main buffer
        readFromBuffer(buffer, delayBuffer, channel);
        //copy input to delay buffer
        fillBuffer(buffer, channel);
        
        }
    
    updateBufferPositions(buffer,delayBuffer);
}



void CircularDelayBufferAudioProcessor::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
    {
        //auto* channelData = buffer.getWritePointer (channel);
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
        if(delayBufferSize > bufferSize + writePosition)
        {
            delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer (channel), bufferSize);
        }
        else
        {
            auto numSamplesToEnd = delayBufferSize - writePosition;
            
            
            delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer (channel), numSamplesToEnd);
            
            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            
            delayBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart);
        }
    }

void CircularDelayBufferAudioProcessor::readFromBuffer(
    juce::AudioBuffer<float>& buffer,
    juce::AudioBuffer<float>& delayBuffer,
    int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    auto delayTime = apvts.getRawParameterValue("delayTime")->load();
    auto delaySamples = (int)(getSampleRate() * delayTime);
    auto wetDry = apvts.getRawParameterValue("wetDry")->load();
    auto dryGain = 1.0f - wetDry;
    auto readPosition = writePosition - delaySamples;
    if (readPosition < 0)
        readPosition += delayBufferSize;
    auto wetGain = 0.7f * wetDry;
    buffer.applyGain(channel, 0, bufferSize, dryGain);

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel,0,delayBuffer.getReadPointer(channel, readPosition),bufferSize,wetGain,wetGain);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel,0,delayBuffer.getReadPointer(channel, readPosition),numSamplesToEnd,wetGain,wetGain);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel,numSamplesToEnd,delayBuffer.getReadPointer(channel, 0),numSamplesAtStart,wetGain,wetGain);
    }
}

void CircularDelayBufferAudioProcessor::updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

//==============================================================================
bool CircularDelayBufferAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CircularDelayBufferAudioProcessor::createEditor()
{
    return new CircularDelayBufferAudioProcessorEditor (*this);
}

//==============================================================================
void CircularDelayBufferAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CircularDelayBufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CircularDelayBufferAudioProcessor();
}
