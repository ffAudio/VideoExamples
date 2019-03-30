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
Properties::Properties()
{
}

Properties::~Properties()
{
}

void Properties::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("Properties", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void Properties::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
