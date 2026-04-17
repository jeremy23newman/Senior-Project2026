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

## **3/10-3/17**
+ This week was also mainly focused on the creation of my presentation.
+ I also started to plan the creation of my final plugin, the plugin would incorporate my past
implementations of delay and filter then also add implementations of overdrive and bitcrush.


## **3/17-3/24**
+ This week was spent learning the core DSP concepts behind overdrive and bitcrush.
+ Overdrive: a form of nonlinear signal processing that pushes a signal or series of samples
behind a threshold to induce clipping or soft saturation which creates harmonic texture.
+ The common DSP function to create overdrive is y[n] = tanh(g*x[n]) where x[n] is the input signal
and y[n] is the output signal.
+ The heavy lifting done in that equation is the tanh which a hyperbolic tangent function used in DSP to simulate
smooth clipping.
+ tanh(z) = (e^z - e^-z)/(e^z+e^-z)
+ Bitcrush: distorts the signal by reducing the resolution or bitdepth and often sample rate, introducing
a grainy, gritty style of sound
+ The key DSP function to create bitcrush is y[n] = 2/2^B * rounded(x[n]/(2/2^B))
+ The B in this formula is what overall changes the tone of the signal the more bits the better resolution and quality of the sound while the fewer bits creates more distortion, manipulating waveforms to produce
a stepwise signal.

## **3/24-3/31**
+ Start final plugin implementation project.
+ I started coding the Overdrive portion, mainly keeping in mind the core concepts that need to recreated on the signal.
+ I broke the equation into two parts, I implemented g*x[n] as auto x = samples[i] * drive, drive is a controllable the more increased the more distorted the signal.
+ Then I took the hyperbolic tangent function which was represented as tanh(g*x[n]) and there was already a
method that has this function so it was simply samples[i] = std::tanh(x), which here we are assigning the distorted signal to the array samples passed to the buffer and I am making use of the precalculted x value.

## **3/31-4/06**
+ Focused on coding the Bitcrusher portion, again just taking the core DSP concepts into code that manipulates
input array of samples.
+ Bitcrush formula like the overdrive was broken up into parts and stored into variables, 2/2^B is represented as
levels = std::pow(2.0f, bitDepth) where bitDepth is controllable parameter.
+ The higher the bitdepth, the better and cleaner audio and the lower the bitdepth more lo-fi/distorted the signal, this is because bitdepth relates heavily to the amplitude and dynamic range. Where if there isn't enough bits to store the amplitude of the signal they compromise the integrity of the audio which causes a more coarse/jagged waveform.
+ After that I take the levels variable and plug it into the parts of the original DSP function left,
rounded(x[n]/(2/2^B)) whichh is represented as std::rounded(x * levels) / levels;
+ Note tha x = samnples[i].

## **4/06-4/13**
+ This week I mainly took half the time designing my user interface for the plugin.
+ Adding knobs for bitcrush, delay, and overdrive then a slider for the filter.
+ I also incorporated some quality of life simple controls like master mix, input gain and output gain.
+ Master Mix: (WET/DRY) the wet signal is the changed signal, the dry signal is the original signal,
I allowed a control for it, because during implementation the combination of all these effects can be
a bit too aggressive of signal manipulation without it.
+ Input/Output Gain: Adjusts the loudness of the signal coming in and leaving the plugin, this helps
a lot in combination with overdrive.

## **4/13-4/20**
+ I dealt with a pretty big bug inside of my delay implementation, which stemmed from the original
implementation sent some of the input signal back into the buffer.
+ Since delay is the last effect in the stack it is influenced by the overdrive which can
boost the original signal beyond a listenable amount.
+ I simply changed the implementation to send a pure wet signal and have a feedback control that indicates
the intensity.
+ I also worked on changing the UI to fit the new knobs that influence the mastermix and different gains.
