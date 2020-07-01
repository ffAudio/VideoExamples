/*
 ==============================================================================
 Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

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

 Overlay component

 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class OSDComponent    : public Component,
                        public Slider::Listener,
                        public Button::Listener,
                        public ChangeListener
{
public:
    OSDComponent (std::shared_ptr<foleys::AVClip> clipToControl, AudioTransportSource* transportToControl)
    : clip (clipToControl), transport (transportToControl)
    {
        setInterceptsMouseClicks (false, true);
        setWantsKeyboardFocus (false);

        openFile.addListener (this);
        openFile.setWantsKeyboardFocus (false);
        addAndMakeVisible (openFile);
        flexBox.items.add (FlexItem (openFile).withFlex (1.0, 1.0, 0.5).withHeight (20.0));

        addAndMakeVisible (seekBar);
        seekBar.addListener (this);
        seekBar.setWantsKeyboardFocus (false);
        flexBox.items.add (FlexItem (seekBar).withFlex (6.0, 1.0, 0.5).withHeight (20.0));

        stop.addListener (this);
        stop.setWantsKeyboardFocus (false);
        addAndMakeVisible (stop);
        flexBox.items.add (FlexItem (stop).withFlex (1.0, 1.0, 0.5).withHeight (20.0));

        pause.addListener (this);
        pause.setWantsKeyboardFocus (false);
        addAndMakeVisible (pause);
        flexBox.items.add (FlexItem (pause).withFlex (1.0, 1.0, 0.5).withHeight (20.0));

        play.addListener (this);
        play.setWantsKeyboardFocus (false);
        addAndMakeVisible (play);
        flexBox.items.add (FlexItem (play).withFlex (1.0, 1.0, 0.5).withHeight (20.0));

        ffwd.addListener (this);
        ffwd.setWantsKeyboardFocus (false);
        addAndMakeVisible (ffwd);
        flexBox.items.add (FlexItem (ffwd).withFlex (1.0, 1.0, 0.5).withHeight (20.0));

        stop.setConnectedEdges  (TextButton::ConnectedOnRight);
        pause.setConnectedEdges (TextButton::ConnectedOnRight | TextButton::ConnectedOnLeft);
        play.setConnectedEdges  (TextButton::ConnectedOnRight | TextButton::ConnectedOnLeft);
        ffwd.setConnectedEdges  (TextButton::ConnectedOnLeft);
    }

    void paint (Graphics& g) override
    {
        if (clip && clip->getLengthInSeconds() > 0)
        {
            g.setColour (Colours::white);
            g.setFont (24);
            auto size = clip->getVideoSize();
            String dim = String (size.width) + " x " + String (size.height);
            g.drawFittedText (dim, getLocalBounds(), Justification::topLeft, 1);
            g.drawFittedText (foleys::timecodeToString (clip->getCurrentTimeInSeconds()),
                              getLocalBounds(), Justification::topRight, 1);
        }
    }

    void resized() override
    {
        Rectangle<int> bounds = getBounds().withTop (getHeight() - 50).reduced (10);
        flexBox.performLayout (bounds);
    }

    void setCurrentTime (const double time)
    {
        seekBar.setValue (time, dontSendNotification);
    }

    void setVideoLength (const double length)
    {
        seekBar.setRange (0.0, length);
    }

    /** React to slider changes with seeking */
    void sliderValueChanged (juce::Slider* slider) override
    {
        if (slider == &seekBar) {
            clip->setNextReadPosition (juce::int64 (slider->getValue() * 48000));
        }
    }

    void buttonClicked (Button* b) override
    {
        if (b == &openFile)
        {
            transport->stop();
            FileChooser chooser ("Open Video File");
            if (chooser.browseForFileToOpen())
            {
                auto video = chooser.getResult();
                if (auto movie = std::dynamic_pointer_cast<foleys::MovieClip> (clip))
                    movie->openFromFile (video);

                if (clip != nullptr)
                    seekBar.setRange (0, clip->getLengthInSeconds());
            }
        }
        else if (b == &play)
        {
            if (ffwdSpeed != 2)
            {
                int64 lastPos = clip->getNextReadPosition();
                ffwdSpeed = 2;
                auto factor = 0.5 + (ffwdSpeed / 4.0);
                transport->setSource (clip.get(), 0, nullptr, factor, 2);
                clip->setNextReadPosition (lastPos);
            }
            transport->start();
        }
        else if (b == &stop)
        {
            transport->stop();
            clip->setNextReadPosition (0);
        }
        else if (b == &pause)
        {
            transport->stop();
        }
        else if (b == &ffwd)
        {
            auto lastPos = clip->getNextReadPosition();
            ffwdSpeed = (ffwdSpeed + 1) % 7;
            auto factor = 0.5 + (ffwdSpeed / 4.0);
            transport->setSource (clip.get(), 0, nullptr, factor, 2);
            clip->setNextReadPosition (lastPos);
            transport->start ();
        }
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        if (clip != nullptr)
            seekBar.setRange (0, clip->getLengthInSeconds());
    }

    class MouseIdle : public MouseListener, public Timer
    {
    public:
        MouseIdle (Component& c) :
        component (c),
        lastMovement (Time::getMillisecondCounter())
        {
            Desktop::getInstance().addGlobalMouseListener (this);
            startTimerHz (20);
        }

        void timerCallback () override
        {
            const auto relTime = Time::getMillisecondCounter() - lastMovement;
            if (relTime < 2000)
            {
                component.setVisible (true);
                component.setAlpha (1.0);
                if (auto* parent = component.getParentComponent())
                    parent->setMouseCursor (MouseCursor::StandardCursorType::NormalCursor);
            }
            else if (relTime < 2300)
            {
                component.setAlpha (1.0f - jmax (0.0f, (relTime - 2000.0f) / 300.0f));
            }
            else
            {
                component.setVisible (false);
                if (auto* parent = component.getParentComponent())
                {
                    parent->setMouseCursor (MouseCursor::StandardCursorType::NoCursor);
                    Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();
                }
            }
        }

        void mouseMove (const MouseEvent &event) override
        {
            if (event.position.getDistanceFrom (lastPosition) > 3.0) {
                lastMovement = Time::getMillisecondCounter();
                lastPosition = event.position;
            }
        }
    private:
        Component&   component;
        int64        lastMovement;
        Point<float> lastPosition;
    };

private:
    MouseIdle       idle      { *this };
    Slider          seekBar   { Slider::LinearHorizontal, Slider::NoTextBox };
    TextButton      openFile  { TRANS ("Open") };
    TextButton      play      { TRANS ("Play") };
    TextButton      pause     { TRANS ("Pause") };
    TextButton      stop      { TRANS ("Stop") };
    TextButton      ffwd      { TRANS ("FWD") };

    FlexBox         flexBox;

    int             ffwdSpeed = 2;
    std::shared_ptr<foleys::AVClip> clip;

    AudioTransportSource* transport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OSDComponent)
};
