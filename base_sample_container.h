#pragma once
#include <vector>

class BaseSampleContainer
{
public:
	BaseSampleContainer(int stateQuantity, int evidence);
	std::vector<double>* getSamplesPTR();
	int getStateQuantity();
	int Get_evidence();
private:
	std::vector<double> _samples;
	int _stateQuantity;			// Number of states
	int _evidence;				// States with values starting at 0
	double _lowerBound;
	double _upperBound;
	void setUpBounds();			// Sets values of lowerbound and upperbound variables
	void generateRandomSamples();
};