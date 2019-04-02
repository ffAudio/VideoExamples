/*
  ==============================================================================

    Properties.cpp
    Created: 30 Mar 2019 4:45:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Properties.h"

//==============================================================================
Properties::Properties (AudioDeviceManager& deviceManagerToUse)
  : deviceManager (deviceManagerToUse),
    selector (deviceManager, 0, 2, 2, 6, false, false, true, false)
{
    addAndMakeVisible (selector);
}

Properties::~Properties()
{
}

void Properties::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void Properties::resized()
{
    selector.setBounds (getLocalBounds().reduced (5));
}
