/*
  ==============================================================================

    TimeLine.h
    Created: 30 Mar 2019 4:46:00pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class TimeLine    : public Component,
                    public DragAndDropTarget,
                    public FileDragAndDropTarget
{
public:
    TimeLine (foleys::VideoEngine& videoEngine, Player& player);
    ~TimeLine();

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override;
    void itemDropped (const SourceDetails &dragSourceDetails) override;

    void paint (Graphics&) override;

    void setEditClip (std::shared_ptr<foleys::AVCompoundClip> clip);
    std::shared_ptr<foleys::AVCompoundClip> getEditClip() const;

    class ClipComponent : public Component
    {
    public:
    private:
        std::unique_ptr<foleys::FilmStrip> filmstrip;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipComponent)
    };

private:

    void addClipToEdit (juce::File file, double start);

    foleys::VideoEngine& videoEngine;
    Player& player;

    std::shared_ptr<foleys::AVCompoundClip> edit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeLine)
};
