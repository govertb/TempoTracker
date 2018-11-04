/*
  ==============================================================================

    ScheirerBand.cpp
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

#include "ScheirerBand.h"

ScheirerBand::ScheirerBand()
{
    for (int channel = 0; channel < 2; ++channel)
        filterArray.add(new juce::IIRFilter());
}

void ScheirerBand::setCoefficients(juce::IIRCoefficients coefs)
{
    for (auto filter : filterArray)
    {
        filter->setCoefficients(coefs);
        filter->reset();
    }
}

void ScheirerBand::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    for (auto filter : filterArray)
        filter->reset();
    envelope_target.get()->prepareToPlay(sampleRate, estimatedSamplesPerBlock);
}

void ScheirerBand::processBlock(juce::AudioSampleBuffer& buffer)
{
    const int numChannels = buffer.getNumChannels();

    while (numChannels > filterArray.size())
        filterArray.add(new juce::IIRFilter(*filterArray.getUnchecked(0)));

    for (int channel = 0; channel < numChannels; ++channel)
        filterArray.getUnchecked(channel)
        ->processSamples(buffer.getWritePointer (channel, 0),
                         buffer.getNumSamples());
    envelope_target.get()->processBlock(buffer);
}