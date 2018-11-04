/*
  ==============================================================================

    ScheierBeatTracker.cpp
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


#include "ScheirerBeatTracker.h"
#include "ScheirerBand.h"
#include "ScheirerEnvelope.h"
#include "ScheirerResonator.h"
#include "ScheirerEngergyFetcher.h"

// First is lowpass, last is highpass, rest is bandpass.
const int num_bands = 6;
const int band_freqs[num_bands] = {200, 300, 600, 1200, 2400, 3200};
const double Q = 3.0;
const double gain = 2.0;

// For the resonators...
const int num_resonators = 20;
const float reso_freqs[num_resonators] =
    {50.,   52.97630324,   56.12977409,   59.47095866,
    63.01103079,   66.76182949,   70.73589848,   74.94652815,
    79.40780004,   84.13463388,   89.14283754,   94.44915986,
    100.07134667,  106.02820013,  112.33964163,  119.02677841,
    126.11197413,  133.61892367,  141.57273237,  150.};


const float half_energy_time = 1500; // in ms.

ScheirerBeatTracker::ScheirerBeatTracker()
{
    // We create each main processing band.
    for (int band_i = 0; band_i < num_bands; ++band_i)
    {
        ScheirerBand *band = new ScheirerBand();
        bands.insert(band_i, band);

        // Attach an Envelope Extractor to it.
        ScheirerEnvelope *band_envelope = new ScheirerEnvelope();
        band->envelope_target = band_envelope;
        if (band_i == 0)
            band_envelope->shouldWrite = true;

        // Attach the resonators to each envelope
        for (int reso_i = 0; reso_i < num_resonators; ++reso_i)
        {
            const float period = 60.0 / reso_freqs[reso_i] * 1000.0; // in ms.
            const float a = std::pow(0.5, (half_energy_time/period));
            ScheirerResonator *resonator = new ScheirerResonator(period, a);
            band_envelope->resonators.insert(reso_i, resonator);

            // We associate each resonator with an integrator
            // if this is the first band we still need to create the integrator.
            if (band_i == 0)  fetchers.insert(reso_i, new ScheirerEnergyFetcher(reso_freqs[reso_i]));
            ScheirerEnergyFetcher *fetcher = fetchers.getUnchecked(reso_i);
            fetcher->integrands.insert(reso_i, resonator);
        }
    }
}

void ScheirerBeatTracker::prepareToPlay(double sampleRate, int samplesPerBlockExpected)
{
    for (int band_i = 0; band_i < num_bands; ++band_i)
    {
        ScheirerBand *band = bands.getUnchecked(band_i);
        if (band_i == 0)
            band->setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, band_freqs[band_i]));
        else if (band_i == num_bands - 1)
            band->setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, band_freqs[band_i]));
        else
            band->setCoefficients(juce::IIRCoefficients::makePeakFilter(sampleRate, band_freqs[band_i], gain, Q));
        band->prepareToPlay(sampleRate, samplesPerBlockExpected);
    }

    // Filters will propagate the prepareToPlay() call.
}

void ScheirerBeatTracker::processBlock(juce::AudioSampleBuffer &bufferToProcess)
{
    juce::AudioSampleBuffer bufferCopy;
    for (int band_i = 0; band_i < num_bands; ++band_i)
    {
        bufferCopy = bufferToProcess;
        bands.getUnchecked(band_i)->processBlock(bufferCopy);
    }
}

float ScheirerBeatTracker::getBPM()
{
    // Peak-picking
    float curTopFreq = 0.0;
    float curTopEnergy = 0.0;
    for (auto fetcher : fetchers) {
        if (fetcher->getEnergy() > curTopEnergy) {
            curTopEnergy = fetcher->getEnergy();
            curTopFreq = fetcher->getFreqIntegrating();
        }
    }
    return curTopFreq;
}
