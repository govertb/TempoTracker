/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "bpmPoller.h"
#include "ScheirerResonator.h"
#include "ScheirerBeatTracker.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                               public ChangeListener,
                               public Button::Listener
{
public:
    //==============================================================================
    MainContentComponent()
    : bpmPoller(bpmValueLabel, beatTracker), state(Stopped)
    {
        addAndMakeVisible(&bpmLabel);
        bpmLabel.setText("BPM:", juce::NotificationType::dontSendNotification);
        bpmLabel.setJustificationType(juce::Justification::centred);
        bpmLabel.setFont(Font(30));


        addAndMakeVisible(&bpmValueLabel);
        bpmValueLabel.setText("0.00", juce::NotificationType::dontSendNotification);
        bpmValueLabel.setJustificationType(juce::Justification::centred);
        bpmValueLabel.setFont(Font(45));

        addAndMakeVisible (&loadButton);
        loadButton.setButtonText ("Load File...");
        loadButton.setColour(TextButton::buttonColourId, Colours::white);
        loadButton.addListener (this);

        addAndMakeVisible (&transportButton);
        transportButton.setImages(false, true, true,
                                  playImage, 1.0, Colours::transparentBlack,
                                  playImage, 1.0, Colours::transparentBlack,
                                  playImage, 1.0, Colours::transparentBlack);
        transportButton.addListener(this);
        transportButton.setEnabled (false);


        setSize (250, 350);

        formatManager.registerBasicFormats();
        transportSource.addChangeListener (this);

        setAudioChannels (0, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //=======================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
        beatTracker.prepareToPlay(sampleRate, samplesPerBlockExpected);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        if (readerSource == nullptr || state == Stopped)
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }
        else
        {
            transportSource.getNextAudioBlock(bufferToFill);
        }

        AudioSourceChannelInfo bufferCopy =
        AudioSourceChannelInfo(AudioSourceChannelInfo(bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples));
        beatTracker.processBlock(*bufferCopy.buffer);
    }

    void releaseResources() override
    {
        transportSource.releaseResources();
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colour::fromRGB(255, 255, 255));

        // You can add your drawing code here!
    }

    void resized() override
    {
        bpmLabel.setBounds(10, 10, getWidth() - 20, 40);
        bpmValueLabel.setBounds(10, 60, getWidth() - 20, 40);
        transportButton.setBounds (30, 120, getWidth() - 60, 180);
        loadButton.setBounds (10, getHeight()- 30, getWidth() - 20, 20);
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &transportSource)
        {
            if (transportSource.isPlaying())
                changeState (Playing);

            else if (transportSource.hasStreamFinished())
            {
                transportSource.setPosition(0.0);
                bpmPoller.reset();
                changeState(Stopped);
            }

            else
                changeState (Stopped);
        }
    }

    void buttonClicked (Button* button) override
    {
        if (button == &loadButton)      loadButtonClicked();
        if (button == &transportButton) transportButtonClicked();
    }

    void updateLoopState (bool shouldLoop)
    {
        if (readerSource != nullptr)
            readerSource->setLooping (shouldLoop);
    }


private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    void changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
                case Stopped:
                    transportButton.setImages(false, true, true,
                                              playImage, 1.0, Colours::transparentBlack,
                                              playImage, 1.0, Colours::transparentBlack,
                                              playImage, 1.0, Colours::transparentBlack);
                    break;

                case Starting:
                    transportSource.start();
                    bpmPoller.startTimer(pollingInvervalMs);
                    break;

                case Playing:
                    transportButton.setImages(false, true, true,
                                              pauseImage, 1.0, Colours::transparentBlack,
                                              pauseImage, 1.0, Colours::transparentBlack,
                                              pauseImage, 1.0, Colours::transparentBlack);
                    break;

                case Stopping:
                    transportSource.stop();
                    bpmPoller.stopTimer();
                    break;
            }
        }
    }

    void loadButtonClicked()
    {
        FileChooser chooser ("Select a Wave file to play...",
                             File::nonexistent,
                             "*.wav");

        if (chooser.browseForFileToOpen())
        {
            File file (chooser.getResult());
            AudioFormatReader* reader = formatManager.createReaderFor (file);

            if (reader != nullptr)
            {
                ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource (reader, true);
                transportSource.setSource (newSource, 0, nullptr, reader->sampleRate);
                transportButton.setEnabled (true);
                readerSource = newSource.release();
                bpmPoller.reset();
                changeState(Stopped);
            }
        }
    }

    void transportButtonClicked()
    {
        switch (state) {
            case Playing:
                changeState(Stopping);
                break;
            case Stopped:
                changeState(Starting);
                break;
            default:
                break;
        }
    }

    //==========================================================================
    TextButton loadButton;
    ImageButton transportButton;
    Label bpmLabel;
    Label bpmValueLabel;
    Image playImage = Image(ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize));
    Image pauseImage = Image(ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize));


    BpmPoller bpmPoller;
    int pollingInvervalMs = 500;

    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;

    ScheirerBeatTracker beatTracker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
