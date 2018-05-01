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
    
    frequencySliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "frequencySliderID", frequencySlider);

    widthSliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "widthSliderID", widthSlider);

    SineSliderAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.parameters, "SineSliderID", SineSlider);
    
    frequencySlider.setSliderStyle(Slider::SliderStyle::Rotary);
    frequencySlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    frequencySlider.setRange(-100, 0);
    frequencySlider.addListener(this);
    addAndMakeVisible(frequencySlider);
    
    widthSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    widthSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    widthSlider.setRange(-2000,2000);
    widthSlider.addListener(this);
    addAndMakeVisible(widthSlider);
    
    SineSlider.setSliderStyle(Slider::SliderStyle::IncDecButtons);
    SineSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    SineSlider.setRange(1,512,1);
    SineSlider.addListener(this);
    addAndMakeVisible(SineSlider);
    
    
    
   
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    widthSlider.removeListener(this);
    frequencySlider.removeListener(this);
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
    frequencySlider.setBounds(-20,20,200,150);
    widthSlider.setBounds(120,20,200,150);
    SineSlider.setBounds(50, 190, 200, 150);
}

void NewProjectAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    if(slider == &frequencySlider)
    {
    processor.setSinusoidParameter(CSinusoid::kAmpThresdB, (float)slider->getValue());
    }
    
    if(slider == &widthSlider)
    {
        processor.setSinusoidParameter(CSinusoid::kMultFactor, pow(2.0, ((float)slider->getValue())/1200) );
    }
    if(slider == &SineSlider)
    {
        processor.setSinusoidParameter(CSinusoid::kMaxNSines, (float)slider->getValue());
    }

    
}
