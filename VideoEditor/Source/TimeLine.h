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
    void resized() override;

    void setEditClip (std::shared_ptr<foleys::AVCompoundClip> clip);
    std::shared_ptr<foleys::AVCompoundClip> getEditClip() const;

    class ClipComponent : public Component
    {
    public:
        ClipComponent (TimeLine& tl, std::shared_ptr<foleys::AVCompoundClip::ClipDescriptor> clip, ThreadPool& threadPool);
        void paint (Graphics& g) override;
        void resized() override;

        void mouseMove (const MouseEvent& event) override;
        void mouseDown (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp (const MouseEvent& event) override;

        std::shared_ptr<foleys::AVCompoundClip::ClipDescriptor> clip;

    private:
        enum DragMode
        {
            notDragging, dragPosition, dragLength
        };

        TimeLine& timeline;
        foleys::FilmStrip filmstrip;

        DragMode dragmode = notDragging;
        Point<int> localDragStart;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipComponent)
    };

    int getXFromTime (double seconds) const;
    double getTimeFromX (int pixels) const;

private:

    void addClipToEdit (juce::File file, double start);

    foleys::VideoEngine& videoEngine;
    Player& player;

    std::shared_ptr<foleys::AVCompoundClip> edit;

    std::vector<std::unique_ptr<ClipComponent>> clipComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeLine)
};
