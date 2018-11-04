/*
  ==============================================================================

    ScheirerEngergyFetcher.cpp
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


#include "ScheirerEngergyFetcher.h"

ScheirerEnergyFetcher::ScheirerEnergyFetcher(float freqIntegrating)
    : freqIntegrating(freqIntegrating) {}

float ScheirerEnergyFetcher::getEnergy()
{
    float sum = 0.0;
    for (auto resonator : integrands)
        sum += resonator->getEnergy();
    return sum;
}

float ScheirerEnergyFetcher::getFreqIntegrating()
{
    return freqIntegrating;
}