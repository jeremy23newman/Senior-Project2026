# **NOTES**

## **2/3-2/10**
+I created a basic filter plugin, the plugin utilizes a high pass and low pass filter
controlled by the cutoff frequnecy.
+Low Pass Filter: A low pass filter cuts out the higher end of frequency allowing 
the prevalence of a lower or smoother tone profile.
+High Pass Filter: A high pass filter cuts out the lower end of frequency allowing 
the prevalence of a higher or brighter tone profile.
+I learned that basic first-order low-pass and high-pass filters in digital signal 
processing are modeled after analog RC (resistor–capacitor) circuits.
+In this implementation a majority of the work is done outside of the intial framework classes.
Instead by creating a LowPassHighPassFilter class, I also had to change the initialization of the constructor.
+The implementation also shows that the formula for highpass and lowpass filters are the same,
except for the signed value which acts an indicator for which filter is being used.
+In the future I hope to update the filter for a visual display of the frequency and
allow for a lowpass and highpass filter to occur at the same time.
