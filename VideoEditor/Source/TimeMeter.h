/*
  ==============================================================================

    TimeMeter.h
    Created: 12 Mar 2021 11:12:19pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TimeLine;

//==============================================================================
/*
*/
class TimeMeter  : public juce::Component,
                   public juce::ScrollBar::Listener
{
public:
    TimeMeter (TimeLine& timeline, juce::ScrollBar& scrollbar);
    ~TimeMeter() override;

    void paint (juce::Graphics&) override;

    void scrollBarMoved (juce::ScrollBar* scrollBarThatHasMoved,
                         double newRangeStart) override;

private:
    TimeLine&        timeline;
    juce::ScrollBar& scrollbar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeMeter)
};
