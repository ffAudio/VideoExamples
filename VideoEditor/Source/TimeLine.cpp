/*
  ==============================================================================

    TimeLine.cpp
    Created: 30 Mar 2019 4:46:00pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TimeLine.h"

//==============================================================================
TimeLine::TimeLine()
{
}

TimeLine::~TimeLine()
{
}

void TimeLine::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

bool TimeLine::isInterestedInFileDrag (const StringArray& files)
{
    return true;
}

void TimeLine::filesDropped (const StringArray& files, int x, int y)
{
    
}
