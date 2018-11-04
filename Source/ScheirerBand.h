/*
  ==============================================================================

    ScheirerBand.h
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


#ifndef SCHEIRERBAND_H_INCLUDED
#define SCHEIRERBAND_H_INCLUDED

#include "ScheirerEnvelope.h"

class ScheirerBand
{
public:
    ScheirerBand();

    // Filter specific functions.
    void setCoefficients(juce::IIRCoefficients coefs);
    void processBlock(juce::AudioSampleBuffer& bufferToProcess);

    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);

    // We forward the buffer from here to envelope_taget
    juce::ScopedPointer<ScheirerEnvelope> envelope_target;

private:
    juce::OwnedArray<juce::IIRFilter> filterArray;
};

#endif  // SCHEIRERBAND_H_INCLUDED
