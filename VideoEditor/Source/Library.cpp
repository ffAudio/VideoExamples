/*
  ==============================================================================

    Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
    All rights reserved.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.

  ==============================================================================

    Library.cpp
    Created: 30 Mar 2019 4:45:01pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Library.h"

//==============================================================================
Library::Library (AudioDeviceManager& manager, foleys::VideoEngine& engine)
  : deviceManager (manager),
    videoEngine (engine)
{
    directoryThread.startThread (3);

    tabs.addTab (NEEDS_TRANS ("Movies"), Colours::darkgrey,
                 new MediaList (directoryThread,
                                File::getSpecialLocation (File::userMoviesDirectory),
                                std::make_unique<WildcardFileFilter> ("*", "*", "All")), true);

#if defined (JUCE_MODULE_AVAILABLE_filmstro_av_clip) && JUCE_MODULE_AVAILABLE_filmstro_av_clip==1
    // FILMSTRO:
    videoEngine.getFormatManager().registerFactory ("filmstro", filmstro::FilmstroClip::getFactory());
    tabs.addTab ("Filmstro", Colours::darkgrey,
                 new FilmstroComponent (deviceManager), true);
#endif

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
