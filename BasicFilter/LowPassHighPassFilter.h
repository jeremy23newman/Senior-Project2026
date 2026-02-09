//
//  LowPassHighPassFilter.h
//  BasicFilter - AU
//
//  Created by Jeremy Newman on 2/8/26.
//
#include <vector>
#include "JuceHeader.h"

#ifndef LowPassHighPassFilter_h
#define LowPassHighPassFilter_h

#include <stdio.h>
class LowPassHighPassFilter
{
public:
    void setHighPass(bool highpass);
    void setCutoffFrequency(float cutOffFrequency);
    void setSamplingRate(float samplingRate);
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    
private:
    bool highPass;
    float cutOffFrequency;
    float samplingRate;
    std::vector<float> dnBuffer;
    
    
};

#endif // !LowPassHighPassFilter_h
