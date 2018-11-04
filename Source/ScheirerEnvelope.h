/*
  ==============================================================================

    ScheirerEnvelope.h
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


#ifndef SCHEIRERENVELOPE_H_INCLUDED
#define SCHEIRERENVELOPE_H_INCLUDED

#include "ScheirerResonator.h"

class ScheirerEnvelope
{
public:
    ScheirerEnvelope();
    void processBlock(juce::AudioSampleBuffer& buffer);
    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);

    juce::OwnedArray<juce::IIRFilter> slopeFilters;
    juce::OwnedArray<ScheirerResonator> resonators;

    bool shouldWrite = false;

private:
    juce::ScopedPointer<juce::WavAudioFormat> wavFormat;
    juce::FileOutputStream *outpuTo;
    juce::ScopedPointer<juce::AudioFormatWriter> writer;

    // Neccesary due to frames.
    juce::Array<float> lastSample;
};


#endif  // SCHEIRERENVELOPE_H_INCLUDED