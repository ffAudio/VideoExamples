/*
  ==============================================================================

    TimeMeter.cpp
    Created: 12 Mar 2021 11:12:19pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TimeMeter.h"
#include "TimeLine.h"

//==============================================================================
TimeMeter::TimeMeter (TimeLine& timelineToUse, juce::ScrollBar& scroll)
  : timeline (timelineToUse),
    scrollbar (scroll)
{
    scrollbar.addListener (this);
}

TimeMeter::~TimeMeter()
{
    scrollbar.removeListener (this);
}

void TimeMeter::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).darker());

    g.setColour (juce::Colours::silver);
    g.setFont (14.0f);

    auto left  = scrollbar.getCurrentRangeStart();
    auto start = timeline.getTimeFromX (juce::roundToInt (left));
    auto end   = timeline.getTimeFromX (juce::roundToInt (left + scrollbar.getCurrentRangeSize()));

    auto step = std::ceil (timeline.getTimeFromX (50));

    for (int i = static_cast<int>(start); i < end; i += step)
    {
        auto pos = juce::roundToInt(timeline.getXFromTime (i) - left);
        g.drawVerticalLine (pos, 0, getHeight());
        g.drawText (juce::String (i) + "s", pos + 3, 0, 30, getHeight(), juce::Justification::left);
    }
}

void TimeMeter::scrollBarMoved (juce::ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    juce::ignoreUnused (scrollBarThatHasMoved, newRangeStart);
    repaint();
}
