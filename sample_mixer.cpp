#include "sample_mixer.h"
#include "r_node.h"
#include "truncated_normal.hpp"
#include <iostream>

KaizenLibrary::SampleMixer::SampleMixer()
{
}

KaizenLibrary::SampleMixer::~SampleMixer()
{
}

KaizenLibrary::WMean::WMean()
{
}

KaizenLibrary::WMean::~WMean()
{
}

double KaizenLibrary::WMean::mix(RNode *rNode)
{
	double div = 0;
	double sum = 0;
	double mean = 0;
	double aux = 0;
	int seed = 123456789;

	rNode->cleanStateValues();

	for (int i = 0; i < rNode->getParentsPtr()->size(); i++) {
		div += rNode->getParentsWeightPtr()->at(i);
	}

	int counter = 0;
	
	for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
		for (int j = 0; j < rNode->getParentsPtr()->size(); j++) {
			sum += rNode->getParentsPtr()->at(j)->getSamplesPtr()->at(i) * rNode->getParentsWeightPtr()->at(j);
		}
		
		mean = sum / div;
		
		sum = 0; 

		aux = truncated_normal_ab_sample(mean, rNode->getVariance(), 0, 1, seed);

		rNode->getSamplesPtr()->at(counter) = aux;

		if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(0)) {
			rNode->getStateValuesPtr()->at(0) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(1)) {
			rNode->getStateValuesPtr()->at(1) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(2)) {
			rNode->getStateValuesPtr()->at(2) += 1;
		} else if (rNode->getNumberOfStates() > 3) {
			if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(3)) {
				rNode->getStateValuesPtr()->at(3) += 1;
			} else {
				rNode->getStateValuesPtr()->at(4) += 1;
			}
		}
		counter++;
	}

	rNode->getStateValuesPtr()->at(0) = (double) rNode->getStateValuesPtr()->at(0) / EQUIDISTANT_SAMPLESIZE;
	if(rNode->getNumberOfStates() > 1)
		rNode->getStateValuesPtr()->at(1) = (double) rNode->getStateValuesPtr()->at(1) / EQUIDISTANT_SAMPLESIZE;
	if(rNode->getNumberOfStates() > 2)
		rNode->getStateValuesPtr()->at(2) = (double) rNode->getStateValuesPtr()->at(2) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 3)
		rNode->getStateValuesPtr()->at(3) = (double) rNode->getStateValuesPtr()->at(3) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 4)
		rNode->getStateValuesPtr()->at(4) = (double) rNode->getStateValuesPtr()->at(4) / EQUIDISTANT_SAMPLESIZE;
	
	rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(0));
	if(rNode->getNumberOfStates() > 1)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(1));
	if(rNode->getNumberOfStates() > 2)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(2));
	if (rNode->getNumberOfStates() > 3)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(3));
	if (rNode->getNumberOfStates() > 4)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(4));
	return aux;
}

KaizenLibrary::WMin::WMin()
{
}

KaizenLibrary::WMin::~WMin()
{
}

double KaizenLibrary::WMin::mix(RNode *rNode)
{
	std::cout<< "Entrou em WMIN!!!" << std::endl;
	double highestValue;
	double div = 0;
	double sum = 0;
	double aux = 0;
	double min = 123456789;
	double a = 0;
	double currentMin = 0;
	int seed = 123456789;

	rNode->cleanStateValues();

	for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
		for (int j = 0; j < rNode->getParentsPtr()->size(); j++) {
			div = rNode->getParentsWeightPtr()->at(j) + rNode->getParentsPtr()->size() - 1;
			a = rNode->getParentsPtr()->at(j)->getSamplesPtr()->at(i) * rNode->getParentsWeightPtr()->at(j); 
			for (int k = 0; k < rNode->getParentsPtr()->size(); k++) {
				if (k != j) {
					sum += rNode->getParentsPtr()->at(k)->getSamplesPtr()->at(i);
				}
			}
			
			currentMin = (a + sum) / div;
			if (currentMin < min) {
				min = currentMin;
			}
			sum = 0; 			
		}
		
		aux = truncated_normal_ab_sample(min, rNode->getVariance(), 0, 1, seed);

		int counter = 0;
		rNode->getSamplesPtr()->at(counter) = aux;

		if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(0)) {
			rNode->getStateValuesPtr()->at(0) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(1)) {
			rNode->getStateValuesPtr()->at(1) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(2)) {
			rNode->getStateValuesPtr()->at(2) += 1;
		} else if (rNode->getNumberOfStates() > 3) {
			if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(3)) {
				rNode->getStateValuesPtr()->at(3) += 1;
			} else {
				rNode->getStateValuesPtr()->at(4) += 1;
			}
		}
		counter++;
	}

	rNode->getStateValuesPtr()->at(0) = (double) rNode->getStateValuesPtr()->at(0) / EQUIDISTANT_SAMPLESIZE;		
	if(rNode->getNumberOfStates() > 1)
		rNode->getStateValuesPtr()->at(1) = (double) rNode->getStateValuesPtr()->at(1) / EQUIDISTANT_SAMPLESIZE;
	if(rNode->getNumberOfStates() > 2)
		rNode->getStateValuesPtr()->at(2) = (double) rNode->getStateValuesPtr()->at(2) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 3)
		rNode->getStateValuesPtr()->at(3) = (double) rNode->getStateValuesPtr()->at(3) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 4)
		rNode->getStateValuesPtr()->at(4) = (double) rNode->getStateValuesPtr()->at(4) / EQUIDISTANT_SAMPLESIZE;
	
	rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(0));
	if(rNode->getNumberOfStates() > 1)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(1));
	if(rNode->getNumberOfStates() > 2)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(2));
	if (rNode->getNumberOfStates() > 3)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(3));
	if (rNode->getNumberOfStates() > 4)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(4));
	return aux;
}

KaizenLibrary::WMax::WMax()
{
}

KaizenLibrary::WMax::~WMax()
{
}

double KaizenLibrary::WMax::mix(RNode *rNode)
{
	double highestValue;
	double div = 0;
	double sum = 0;
	double aux = 0;
	double max = 0;
	double a = 0;
	double currentMax = 0;	
	int seed = 123456789;

	rNode->cleanStateValues();

	for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
		for (int j = 0; j < rNode->getParentsPtr()->size(); j++) {
			div = rNode->getParentsWeightPtr()->at(j) + rNode->getParentsPtr()->size() - 1;
			a = rNode->getParentsPtr()->at(j)->getSamplesPtr()->at(i) * rNode->getParentsWeightPtr()->at(j); 
			for (int k = 0; k < rNode->getParentsPtr()->size(); k++) {
				if (k != j) {
					sum += rNode->getParentsPtr()->at(k)->getSamplesPtr()->at(i);
				}
			}
			
			currentMax = (a + sum) / div;
			if (currentMax > max) {
				max = currentMax;
			}
			sum = 0; 			
		}
		
		aux = truncated_normal_ab_sample(max, rNode->getVariance(), 0, 1, seed);

		int counter = 0;
		rNode->getSamplesPtr()->at(counter) = aux;

		if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(0)) {
			rNode->getStateValuesPtr()->at(0) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(1)) {
			rNode->getStateValuesPtr()->at(1) += 1;
		} else if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(2)) {
			rNode->getStateValuesPtr()->at(2) += 1;
		} else if (rNode->getNumberOfStates() > 3) {
			if (rNode->getSamplesPtr()->at(counter) < rNode->getStateIntervalsPtr()->at(3)) {
				rNode->getStateValuesPtr()->at(3) += 1;
			} else {
				rNode->getStateValuesPtr()->at(4) += 1;
			}
		}
		counter++;
	}

	rNode->getStateValuesPtr()->at(0) = (double) rNode->getStateValuesPtr()->at(0) / EQUIDISTANT_SAMPLESIZE;		
	if(rNode->getNumberOfStates() > 1)
		rNode->getStateValuesPtr()->at(1) = (double) rNode->getStateValuesPtr()->at(1) / EQUIDISTANT_SAMPLESIZE;
	if(rNode->getNumberOfStates() > 2)
		rNode->getStateValuesPtr()->at(2) = (double) rNode->getStateValuesPtr()->at(2) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 3)
		rNode->getStateValuesPtr()->at(3) = (double) rNode->getStateValuesPtr()->at(3) / EQUIDISTANT_SAMPLESIZE;
	if (rNode->getNumberOfStates() > 4)
		rNode->getStateValuesPtr()->at(4) = (double) rNode->getStateValuesPtr()->at(4) / EQUIDISTANT_SAMPLESIZE;
	
	rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(0));
	if(rNode->getNumberOfStates() > 1)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(1));
	if(rNode->getNumberOfStates() > 2)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(2));
	if (rNode->getNumberOfStates() > 3)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(3));
	if (rNode->getNumberOfStates() > 4)
		rNode->getTpnPtr()->push_back(rNode->getStateValuesPtr()->at(4));
	return aux;
}

KaizenLibrary::MixMinMax::MixMinMax()
{
}

KaizenLibrary::MixMinMax::~MixMinMax()
{
}

double KaizenLibrary::MixMinMax::mix(RNode *rNode)
{
	// TODO
	return 0;
}

KaizenLibrary::EmptyMixer::EmptyMixer()
{
}

KaizenLibrary::EmptyMixer::~EmptyMixer()
{
}

double KaizenLibrary::EmptyMixer::mix(RNode *rNode) 
{
	return 0;
}