/*
  ==============================================================================

    ScheirerEnvelope.cpp
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "ScheirerEnvelope.h"
#include "ScheirerBand.h"

ScheirerEnvelope::ScheirerEnvelope() {}
void ScheirerEnvelope::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    for (int c = 0; c < 2; ++c)
    {
        juce::IIRFilter *f = new juce::IIRFilter();
        f->setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 7));
        slopeFilters.insert(c, f);
        lastSample.add(0.0);
    }

    for (auto resonator : resonators)
        resonator->prepareToPlay(sampleRate, estimatedSamplesPerBlock);
}

void ScheirerEnvelope::processBlock(juce::AudioSampleBuffer& buffer)
{
    const int numBufferSamples = buffer.getNumSamples();

    // Rectify, Smooth
    for(int c = 0; c < buffer.getNumChannels(); ++c)
    {
        float *buffer_p = buffer.getWritePointer(c);
        FloatVectorOperations::abs(buffer_p, buffer_p, numBufferSamples);
        slopeFilters.getUnchecked(c)->processSamples(buffer_p, numBufferSamples);
    }

    // First-Order Derivative, HW-Rectify
    juce::AudioSampleBuffer bufferCopy = buffer;
    for (int c = 0; c < buffer.getNumChannels(); ++c) {
        float *buffer_p = buffer.getWritePointer(c);
        const float *buffer_c_p = bufferCopy.getReadPointer(c);
        buffer_p[0] -= lastSample.getUnchecked(c);
        lastSample.set(c, buffer_p[numBufferSamples - 1]);
        FloatVectorOperations::subtract(buffer_p + 1, buffer_c_p, numBufferSamples-1);
        FloatVectorOperations::clip(buffer_p, buffer_p, 0.0, std::numeric_limits<float>::max(), numBufferSamples);
        // TODO: Decimate ?
    }

    // Output to the resonators
    for (int reso_id = 0; reso_id < resonators.size(); ++reso_id)
    {
        bufferCopy = buffer;
        resonators.getUnchecked(reso_id)->processBlock(bufferCopy);
    }
}