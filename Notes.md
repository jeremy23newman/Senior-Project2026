# **NOTES**

## **2/3-2/10**
+ I created a basic filter plugin, the plugin utilizes a high pass and low pass filter
controlled by the cutoff frequnecy.
+ Low Pass Filter: A low pass filter cuts out the higher end of frequency allowing 
the prevalence of a lower or smoother tone profile.
+ High Pass Filter: A high pass filter cuts out the lower end of frequency allowing 
the prevalence of a higher or brighter tone profile.
+ I learned that basic first-order low-pass and high-pass filters in digital signal 
processing are modeled after analog RC (resistor–capacitor) circuits.
+ In this implementation a majority of the work is done outside of the intial framework classes.
Instead by creating a LowPassHighPassFilter class, I also had to change the initialization of the constructor.
+ The implementation also shows that the formula for highpass and lowpass filters are the same,
except for the signed value which acts an indicator for which filter is being used.
+ In the future I hope to update the filter for a visual display of the frequency and
allow for a lowpass and highpass filter to occur at the same time.

## **2/10-2/17**
+ I created a basic delay plugin, the implementation was heavily based heavily around
a circular buffer.
+ The circular buffer copies information from our audio buffer into the delay buffer with enough room
to keep prior information from prior cycles
+ In doing so I can send information from prior audio back into the current audio buffer
to induce a delay like sound effect.
+ On top of that you have to incorporate the idea of a feedback loop because without the feedback loop
only one instance of echo occurs.
+ Then with a feedback loop it causes that same singal to be repeated but at lower volumes until
the signal becomes inaudible.
+ Although this implementation is very basic and I will have to take more time to incorporate parameters
like mix(wet vs dry signal), delay time(how long it takes for echo'ed signal to be played) and feedback(the number of repeats).

## **2/17-2/24**
+ Researched more of the core concepts behind the plugin immplementations I have so far.
+ The delay as mentioned is built off of a circular delay buffer, y[n] = x[n] + g * x[n-1]
is the base formula used to represent the application.
+ My implementation more specifically a single-tap feed forward delay, which means it takes
one instance of the sample and combines it with output single to produce a single delay effect.
+ Although with my implementation I incorporate a second layer of logic that produces a feedback loop so
that one instance is duplicated multiple times.
+ My filter implementation is a first order all pass filter, which means that I am not subtracting
samples or information from the signal instead of reinforcing areas so that it sounds more prominent than others.
+ All pass is represented as filterOutput = 0.5f * (inputsamplesign * allPassFilteredSample) -> Input (+ or -) Allpass(Input)
+ LowPass = 1/2(x[n] + A(x[n]))
+ HighPass = 1/2(x[n] - A(x[n]))

## **2/24-3/03**
+ Implemented parameters and user controls for circular feedback delay plugin
+ The features were delay time and wet/dry mix.
+ Delay Time - is the amount of time in between each instance of the sample being repeated,
often determined by multiplying the sample rate(hz) by differe values controlled by the user
+ Wet/Dry Mix - is control of how much of the signal heard by the user is manipulated by the plugin,
allowing the user to control how apparent the delay is in the output signal.
+ Further research on my filtered implementation, a1 = (tan(pifc/fs)-1)/(tan(pifc/fs) + 1
Fc = cutoff frequency and Fs = sample rate
Low Frequencies: All pass = og signal, adding -> reinforces lows, subtracting -> cancels lows
High Frequencies: All pass = inverted signal, adding -> cancels highs, subtracting -> reinforces highs

## **3/03-3/10**
+ This week focused mainly on creating/designing the presentation and preparing my outline.
+ Huge resource for implentation and research:
  [The Audio Programmer Link](https://www.youtube.com/@TheAudioProgrammer)

