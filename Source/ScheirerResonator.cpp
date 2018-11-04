/*
  ==============================================================================

    ScheirerResonator.cpp
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


#include "ScheirerResonator.h"

/* This resonator uses a combfilter. This filter is
   implemented using a circular buffer which, in turn,
   is implemented using a regular buffer (history),
   a read index (history_read), and a write index
   (history_write). */


ScheirerResonator::ScheirerResonator(float delayInMs, float alpha)
    : alpha(alpha), delayInMs(delayInMs) {}

void ScheirerResonator::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    const float delayInSamples = (delayInMs/1000.0) * sampleRate;

    // How much to interpolate (into next history sample)?
    interpolationFactor = delayInSamples - floor(delayInSamples);
    const int history_size = ceil(delayInSamples); // ceil() since we interpolate
    history = juce::AudioSampleBuffer(2, history_size);
    history.clear();

    // history_read is index of sample succeeding the interpolation sample
    history_read  = -history_size + 1;
    history_write = 0;
}

void ScheirerResonator::processBlock(juce::AudioSampleBuffer& buffer)
{
    const int numBufferSamples    = buffer.getNumSamples();
    const int numHistorySamples   = history.getNumSamples();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float *buffer_p  = buffer.getWritePointer(channel, 0);
        float *history_p = history.getWritePointer(channel, 0);

        for (int i = 0; i < numBufferSamples; ++i)
        {
            // We can only incorporate history
            // if we accumulated enough of it.
            if (history_read >= 1)
                buffer_p[i] =
                alpha * (interpolationFactor * history_p[(history_read + i - 1) % numHistorySamples]
                         +
                         (1-interpolationFactor)*history_p[(history_read + i) % numHistorySamples])
                +
                (1-alpha)*buffer_p[i];

            // Maintain delayline history
            history_p[(history_write + i) % numHistorySamples] = buffer_p[i];
        }
    }

    history_read  = (history_read  + numBufferSamples) % numHistorySamples;
    history_write = (history_write + numBufferSamples) % numHistorySamples;
}

float ScheirerResonator::getEnergy(){
    return history.getMagnitude(0, history.getNumSamples()-1);

}
