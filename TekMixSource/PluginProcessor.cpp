/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
TekMixAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //DELAY
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "delayTime", "Delay Time",
                                                                 juce::NormalisableRange<float>(0.01f, 2.0f, 0.01f), 0.5f));
    
    //FEEDBACK
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "feedback", "Feedback",
                                                                 juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.3f));
    
    //FILTER
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "cutoff", "Cutoff",
                                                                 juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f), 1000.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
                                                                "highpass", "High Pass", false));
    
    //OVERDRIVE
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "drive", "Drive",
                                                                 juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 2.0f));
    
    //BITCRUSHER
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "bitDepth", "Bit Depth",
                                                                 juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f), 8.0f));
    
    //GLOBAL
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "inputGain", "Input Gain",
                                                                 juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "outputGain", "Output Gain",
                                                                 juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                                                                 "masterMix", "Master Mix",
                                                                 juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
                                                                "bypassDelay", "Bypass Delay", false));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
                                                                "bypassBitcrush", "Bypass Bitcrush", false));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
                                                                "bypassOverdrive", "Bypass Overdrive", false));
    
    
    return { params.begin(), params.end() };
}

//==============================================================================
// CONSTRUCTOR
TekMixAudioProcessor::TekMixAudioProcessor()
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

TekMixAudioProcessor::~TekMixAudioProcessor()
{
}

//==============================================================================
const juce::String TekMixAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TekMixAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TekMixAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TekMixAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TekMixAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TekMixAudioProcessor::getNumPrograms()
{
    return 1;
    
}

int TekMixAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TekMixAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TekMixAudioProcessor::getProgramName (int index)
{
    return {};
}

void TekMixAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TekMixAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    samplingRate = sampleRate;
    
    //DELAY BUFFER INTIALIZED
    auto delayBufferSize = (int)(sampleRate * 2.0);
    delayBuffer.setSize(getTotalNumOutputChannels(), delayBufferSize);
    delayBuffer.clear();
    
    //FILTER STARTING POSITION
    dnBuffer.resize(getTotalNumOutputChannels(), 0.0f);
    
    writePosition = 0;
    
}

void TekMixAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TekMixAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TekMixAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //STORE DRY SIGNAL FOR MASTER MIX
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    //INPUT GAIN
    auto inputGain = apvts.getRawParameterValue("inputGain")->load();
    buffer.applyGain(inputGain);
    
    processOverdrive(buffer);
    processFilter(buffer);
    processBitcrush(buffer);
    processDelay(buffer);
    
    //MASTER MIX
    auto mix = apvts.getRawParameterValue("masterMix")->load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto wet = buffer.getSample(ch, i);
            auto dry = dryBuffer.getSample(ch, i);
            
            buffer.setSample(ch, i, dry * (1.0f - mix) + wet * mix);
        }
    }
    
    //OUTPUT GAIN
    auto outputGain = apvts.getRawParameterValue("outputGain")->load();
    buffer.applyGain(outputGain);
    
    
}
//OVERDRIVE
void TekMixAudioProcessor::processOverdrive(juce::AudioBuffer<float>& buffer)
{
    if (apvts.getRawParameterValue("bypassOverdrive")->load() > 0.5f)
        return;
    
    auto drive = apvts.getRawParameterValue("drive")->load();
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto x = samples[i] * drive;
            
            
            samples[i] = std::tanh(x);
        }
    }
}
//BITCRUSH
void TekMixAudioProcessor::processBitcrush(juce::AudioBuffer<float>& buffer)
{
    if (apvts.getRawParameterValue("bypassBitcrush")->load() > 0.5f)
        return;
    
    auto bitDepth = apvts.getRawParameterValue("bitDepth")->load();
    
    auto levels = std::pow(2.0f, bitDepth);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto x = samples[i];
            
            
            samples[i] = std::round(x * levels) / levels;
        }
    }
}
//FILTER
void TekMixAudioProcessor::processFilter(juce::AudioBuffer<float>& buffer)
{
    constexpr auto PI = 3.14159265359f;
    
    auto cutoff = apvts.getRawParameterValue("cutoff")->load();
    auto hp = apvts.getRawParameterValue("highpass")->load();
    
    const auto sign = hp ? -1.f : 1.f;
    
    const auto tanVal = std::tan(PI * cutoff / samplingRate);
    const auto a1 = (tanVal - 1.f) / (tanVal + 1.f);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto input = samples[i];
            
            auto allpass = a1 * input + dnBuffer[channel];
            dnBuffer[channel] = input - a1 * allpass;
            
            samples[i] = 0.5f * (input + sign * allpass);
        }
    }
}
//DELAY
void TekMixAudioProcessor::processDelay(juce::AudioBuffer<float>& buffer)
{
    if (apvts.getRawParameterValue("bypassDelay")->load() > 0.5f)
        return;
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        fillBuffer(buffer, channel);
        readFromBuffer(buffer, delayBuffer, channel);
    }
    
    updateBufferPositions(buffer, delayBuffer);
}


void TekMixAudioProcessor::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    if (delayBufferSize > bufferSize + writePosition)
    {
        delayBuffer.copyFrom(channel, writePosition,
                             buffer.getWritePointer(channel), bufferSize);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - writePosition;
        
        delayBuffer.copyFrom(channel, writePosition,
                             buffer.getWritePointer(channel), numSamplesToEnd);
        
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        
        delayBuffer.copyFrom(channel, 0,
                             buffer.getWritePointer(channel, numSamplesToEnd),
                             numSamplesAtStart);
    }
}

void TekMixAudioProcessor::readFromBuffer(
                                          juce::AudioBuffer<float>& buffer,
                                          juce::AudioBuffer<float>& delayBuffer,
                                          int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    auto delayTime = apvts.getRawParameterValue("delayTime")->load();
    auto delaySamples = (int)(getSampleRate() * delayTime);
    
    auto feedback = apvts.getRawParameterValue("feedback")->load();
    
    auto readPosition = writePosition - delaySamples;
    if (readPosition < 0)
        readPosition += delayBufferSize;
    
    
    for (int i = 0; i < bufferSize; ++i)
    {
        int delayIndex = (readPosition + i) % delayBufferSize;
        int writeIndex = (writePosition + i) % delayBufferSize;
        
        auto delayedSample = delayBuffer.getSample(channel, delayIndex);
        auto inputSample   = buffer.getSample(channel, i);
        
        
        buffer.setSample(channel, i, delayedSample);
        
        
        delayBuffer.setSample(channel, writeIndex,
                              inputSample + delayedSample * feedback);
    }
}

void TekMixAudioProcessor::updateBufferPositions(
                                                 juce::AudioBuffer<float>& buffer,
                                                 juce::AudioBuffer<float>& delayBuffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

//==============================================================================
bool TekMixAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TekMixAudioProcessor::createEditor()
{
    return new TekMixAudioProcessorEditor (*this);
}

//==============================================================================
void TekMixAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TekMixAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TekMixAudioProcessor();
}
