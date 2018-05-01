# Sinusoidal-Modelling

Sinusoidal modelling is a technique used to approximate a signal, in audio it is typically used in the task of speech analysis and speech synthesis. In our project we use sinusoidal models to deconstruct the audio and morph it in different ways by changing the number of sine waves approximating the signal, changing the pitch and changing the amplitude threshold for detecting peaks.

## Third party tools
* UtilFunctions -- Used to generate sine waves (Source: https://github.com/MTG/sms-tools/tree/master/software/models/utilFunctions_C)
* FFT -- Used for Computing FFT and IFFT (Source: https://github.com/GTCMT/2018-MUSI6106/tree/master/3rdparty/Fft)

## Steps to build
* Open the Jucer (SineModelJuce/NewProject.jucer).
* Set the header search path to /Sinusoidal-Modelling/src/inc (absolute path from root directory).
* Add Sinudoid.cpp, Fft.cpp, utilFunctions.c and rvfft.cpp to the JUCE Source folder (click on + and add existing files)
* Set JUCE Modules path.
* Click the "Save and open in IDE" button in JUCER.

## Verification
* Sine wave, Zero (DC), speech and music audio files and compared with the python implementation.

## Authors
* Rupak Vignesh Swaminathan
* Vinod Subramanian

## Acknowledgements
* Alexander Lerch (Audio Software Engineering instructor)
* Takahiko Tsuchiya (Audio Software Engineering TA)
* Xavier Serra (Python Reference Implementation)
