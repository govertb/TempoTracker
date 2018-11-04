/*
  ==============================================================================

    bpmPoller.h
    Copyright © 2015  Govert Brinkmann

    This file is part of TempoTracker.

    TempoTracker is free software: you can redistribute it and/or modify
    it under the terms of version 3 of the GNU Affero General Public License as
    published by the Free Software Foundation.

    TempoTracker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with TempoTracker.  If not, see <https://www.gnu.org/licenses/>.

    This code was written as part of the Audio Processing & Indexing Course
    (2015-2016) at Leiden University.

  ==============================================================================
*/

#ifndef BPMPOLLER_H_INCLUDED
#define BPMPOLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ScheirerBeatTracker.h"

class BpmPoller : public juce::Timer
{
public:
    BpmPoller(juce::Label &labelToUpdate, ScheirerBeatTracker &beatTracker);
    virtual void timerCallback () override;
    void reset();

private:
    juce::Label &labelToUpdate;
    ScheirerBeatTracker &beatTracker;
    std::unordered_map<float, int> pastTempi;
};

#endif  // BPMPOLLER_H_INCLUDED
