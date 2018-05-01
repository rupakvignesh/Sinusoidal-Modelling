/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider *slider) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& processor;

     Slider thresholdSlider, pitchSlider, SineSlider;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttach;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> pitchSliderAttach;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> SineSliderAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
