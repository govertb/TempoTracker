/*
  ==============================================================================

    bpmPoller.cpp
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

#include "bpmPoller.h"
#include <sstream>

BpmPoller::BpmPoller(juce::Label &labelToUpdate, ScheirerBeatTracker &beatTracker)
    : labelToUpdate(labelToUpdate), beatTracker(beatTracker) {}

void BpmPoller::timerCallback()
{
    float cur_tempo = beatTracker.getBPM();
    pastTempi[cur_tempo] += 1;

    float mode_tempo = 0.0;
    int mode_tempo_freq = 0;
    for (auto it : pastTempi) {
        if (it.second > mode_tempo_freq)
        {
            mode_tempo = it.first;
            mode_tempo_freq = it.second;
        }
    }

    juce::String str = juce::String::formatted("%.2f", mode_tempo);
    labelToUpdate.setText(str, juce::NotificationType::dontSendNotification);
}

void BpmPoller::reset()
{
    pastTempi.clear();
}
