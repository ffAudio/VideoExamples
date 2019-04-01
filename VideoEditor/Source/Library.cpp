/*
  ==============================================================================

    Library.cpp
    Created: 30 Mar 2019 4:45:01pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Library.h"

//==============================================================================
Library::Library()
{
    directoryThread.startThread (3);

    tabs.addTab (NEEDS_TRANS ("Movies"), Colours::darkgrey,
                 new MediaList (directoryThread,
                                File::getSpecialLocation (File::userMoviesDirectory),
                                std::make_unique<WildcardFileFilter> ("*", "*", "All")), true);

    tabs.addTab (NEEDS_TRANS ("Music"), Colours::darkgrey,
                 new MediaList (directoryThread,
                                File::getSpecialLocation (File::userMusicDirectory),
                                std::make_unique<WildcardFileFilter> ("*", "*", "All")), true);

    tabs.addTab (NEEDS_TRANS ("Stills"), Colours::darkgrey,
                 new MediaList (directoryThread,
                                File::getSpecialLocation (File::userPicturesDirectory),
                                std::make_unique<WildcardFileFilter> ("*", "*", "All")), true);

    addAndMakeVisible (tabs);
}

Library::~Library()
{
}

void Library::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void Library::resized()
{
    tabs.setBounds (getLocalBounds().reduced (3));
}

//==============================================================================
Library::MediaList::MediaList (TimeSliceThread& readThread, const File& root, std::unique_ptr<FileFilter> filterToUse)
  : filter (std::move (filterToUse)),
    contents (nullptr, readThread)
{
    contents.setDirectory (root, true, true);
    if (filter)
        contents.setFileFilter (filter.get());

    fileTree.setDragAndDropDescription ("media");

    addAndMakeVisible (fileTree);
}

void Library::MediaList::resized()
{
    fileTree.setBounds (getLocalBounds());
}
