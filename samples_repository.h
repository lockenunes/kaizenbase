#pragma once
#include <vector>

class BasicSample;

class SamplesRepository {

public:
	SamplesRepository();
	virtual ~SamplesRepository();

	static std::vector<BasicSample> basicSampleContainer;
	// Container for basic samples

	static void addBasicSample(double mean, int states, std::vector<double> samples);
	// Adds a new sample to the basic samples container (for reuse)

	static std::vector<double> reuseBasicSample(double mean, int states);
	// Returns previously used samples
	// Returns an empty vector in case no matching sample has been used

};