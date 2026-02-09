//
//  LowPassHighPassFilter.cpp
//  BasicFilter - AU
//
//  Created by Jeremy Newman on 2/8/26.
//

#include "LowPassHighPassFilter.h"

void LowPassHighPassFilter::setHighPass(bool highPass)
{
    this-> highPass = highPass;
}

void LowPassHighPassFilter::setCutoffFrequency(float cutOffFrequency)
{
    this->cutOffFrequency = cutOffFrequency;
}

void LowPassHighPassFilter::setSamplingRate(float samplingRate)
{
    this->samplingRate = samplingRate;
}

void LowPassHighPassFilter::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    constexpr auto PI = 3.14159265359f;
    dnBuffer.resize(buffer.getNumChannels(), 0.f);
    const auto sign = highPass ? -1.f : 1.f;
    
    const auto tan = std::tan(PI * cutOffFrequency / samplingRate);
    const auto a1 = (tan - 1.f) / (tan + 1.f);
    
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto channelSamples = buffer.getWritePointer(channel);
        for (auto i = 0; i < buffer.getNumSamples(); ++i)
        {
            const auto inputSample = channelSamples[i];
            
            const auto allPassFilteredSample = a1 * inputSample + dnBuffer[channel];
            
            dnBuffer[channel] = inputSample - a1 * allPassFilteredSample;
            
            const auto filterOutput = 0.5f * (inputSample + sign * allPassFilteredSample);
            
            channelSamples[i] = filterOutput;
            
            
        };
        
    }
    
    
    
    
}
