/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 400);
    
    thresholdSliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "thresholdSliderID", thresholdSlider);

    pitchSliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "pitchSliderID", pitchSlider);

    SineSliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "SineSliderID", SineSlider);
    
    thresholdSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    thresholdSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    thresholdSlider.setRange(-100, 0);
    thresholdSlider.addListener(this);
    addAndMakeVisible(thresholdSlider);
    
    pitchSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    pitchSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    pitchSlider.setRange(-1200,1200,6);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);
    
    SineSlider.setSliderStyle(Slider::SliderStyle::IncDecButtons);
    SineSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    SineSlider.setRange(1,512,1);
    SineSlider.addListener(this);
    addAndMakeVisible(SineSlider);
    
    
    
   
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    pitchSlider.removeListener(this);
    thresholdSlider.removeListener(this);
    SineSlider.removeListener(this);
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Threshold (dB)", 30, 170, 100, 25, Justification::centred, 1);
    g.drawFittedText ("Pitch shift (cents)", 170, 170, 100, 25, Justification::centred, 1);
    g.drawFittedText("Num Sines", 100,340, 100, 25, Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdSlider.setBounds(-20,20,200,150);
    pitchSlider.setBounds(120,20,200,150);
    SineSlider.setBounds(50, 190, 200, 150);
}

void NewProjectAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    if(slider == &thresholdSlider)
    {
    processor.setSinusoidParameter(CSinusoid::kAmpThresdB, (float)slider->getValue());
    }
    
    if(slider == &pitchSlider)
    {
        processor.setSinusoidParameter(CSinusoid::kMultFactor, pow(2.0, ((float)slider->getValue())/1200) );
    }
    if(slider == &SineSlider)
    {
        processor.setSinusoidParameter(CSinusoid::kMaxNSines, (float)slider->getValue());
    }

    
}
