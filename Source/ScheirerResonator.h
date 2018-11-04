/*
  ==============================================================================

    ScheirerResonator.h
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

#ifndef SCHEIRERRESONATOR_H_INCLUDED
#define SCHEIRERRESONATOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class ScheirerResonator
{
public:
    ScheirerResonator(float delayInMs, float alpha);

    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
    void processBlock(juce::AudioSampleBuffer& buffer);
    float getEnergy();

private:
    // History used for delayline and energy accumulation.
    juce::AudioSampleBuffer history;
    int history_write = 0;
    int history_read  = 0;
    float interpolationFactor = 0.0;

    // Filter Parameters
    float alpha;
    float delayInMs;
};

#endif  // SCHEIRERRESONATOR_H_INCLUDED
