#include "r_node.h"
#include <iostream>
#include "base_sample_service.h"
#include "truncated_normal.hpp"
#include "global.h"
#include "samples_repository.h"
#include "sample_mixer.h"

using namespace std;

namespace KaizenLibrary
{
	// Problema com esta variável estática resolvido, solução no link:
	// http://forums.codeguru.com/showthread.php?361278-error-c2491-help-me-thanks-everyone!!
	int RNode::current_id;

	RNode::RNode() : _id(current_id++)
	{
		_evidence = -1;
		wExpression = WExpression::WMEAN;
		_sampleMixer = new WMean;		// SampleMixer default
	}

	RNode::RNode(int id)
	{
		this->_id = id;
		_nStates = 0;
		_mu = DEFAULT_MU;
		_var = DEFAULT_VAR;
		_evidence = -1; // TODO Case 1
		wExpression = WExpression::WMEAN;
	}

	// @deprecated
	void RNode::Init3StateNode(string state1, string state2, string state3, double inVar, double inMu)
	{
		_nStates = 3;
		_mu = inMu;
		_var = inVar;
	
		_stateTitles.reserve(_nStates);
		_stateTitles.resize(_nStates);

		_stateTitles.at(0) = state1.c_str();
		_stateTitles.at(1) = state2.c_str();
		_stateTitles.at(2) = state3.c_str();
	
		this->init();
	}

	void RNode::init()
	{	
		_nStates = _stateTitles.size();
		_samples.resize(SAMPLESIZE);
		
		_stateValues.resize(_nStates);
		double stateValue = (double) 1 / _nStates; 
		_stateIntervals.clear();

		for (int i = 1; i <= _nStates; i++) {
			_stateIntervals.push_back((double) i / _nStates);
			_stateValues.at(i-1) = stateValue;
		}

	}

	RNode::~RNode()
	{
	}

	void RNode::generateEvidence(double inMu)
	{
		int seed = 123456789;
		double aux;

		for (int i = 0; i < SAMPLESIZE; i++) {
			aux = truncated_normal_ab_sample(inMu, .0001, LOWER_BOUND, UPPER_BOUND, seed);
			_samples.at(i) = aux;
		}
	}

	// Util
	void RNode::print() {
		cout << _name << endl;
		cout << "---------------------" << endl;
		for (int i = 0; i < _nStates; i++)
			cout << _stateTitles.at(i) << ": " << _stateValues.at(i) << endl;
		cout << "---------------------\n";
	}

	// Core
	// @deprecated
	void RNode::vertigo(int parentIndex)
	{
		double tMean;
		for (int i = 0; i < _parents.at(parentIndex)->_nStates; i++) {
			if(_parents.at(parentIndex)->_nStates == 1) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .500000;
			} else if (_parents.at(parentIndex)->_nStates == 2)	{
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .250000;
			} else if (_parents.at(parentIndex)->_nStates == 3) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .166667;
			} else if (_parents.at(parentIndex)->_nStates == 4) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .125000;
			} else {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .1;
			}
		
			vector<double> tSamples = SamplesRepository::reuseBasicSample(tMean, _parents.at(parentIndex)->_nStates);
			if (tSamples.size() == 0) {
				_parents.at(parentIndex)->generateEvidence(tMean);
				SamplesRepository::addBasicSample(tMean, _parents.at(parentIndex)->_nStates, _parents.at(parentIndex)->_samples);
			} else {
				_parents.at(parentIndex)->_samples = tSamples;
			}
		
			if (parentIndex < _parents.size()) {
				/* Is it the last parent? */
				if ((parentIndex + 1) == _parents.size()) {
					/* Mix samples using expression */
					switch(wExpression) {
						case WExpression::WMIN : 
							weigthedMin();
							break;
						case WExpression::WMEAN : 
							weigthedMean();
							break;
						case WExpression::WMAX : 
							weigthedMax();
							break;
						case WExpression::MIXMINMAX : 
							mixMinMax();
							break;
					}
				
					for (int z = 0; z < _nStates; z++) {
						_tpn.push_back(_stateValues.at(z));
					}

				} else {
					vertigo(parentIndex + 1);
				}
			}
		}
	}

	// Não está sendo utilizado TODO Verificar necessidade.
	void RNode::resetStateValues()
	{
		_stateValues.clear();
		clog << ">>>Resetando valores dos estados do RNode...";
		double value = (double) 1 / _nStates;

		for (int i = 0; i < _nStates; i++) {
			_stateValues.push_back(value);
		}
	
	}

	// @deprecated
	void RNode::weigthedMean()
	{
		double div = 0;
		double sum = 0;
		double mean = 0;
		double aux = 0;
		int seed = 123456789;

		for (int i = 0; i < _parents.size(); i++) {
			div += _parentsWeight.at(i);
		}

		int counter = 0;
	
		for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
			for (int j = 0; j < _parents.size(); j++) {
				sum += _parents.at(j)->_samples.at(i) * _parentsWeight.at(j);
			}
		
			mean = sum / div;
		
			sum = 0; 

			aux = truncated_normal_ab_sample(mean, _var, 0, 1, seed);

			_samples.at(counter) = aux;

			if (_samples.at(counter) < _stateIntervals.at(0)) {
				_stateValues.at(0) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(1)) {
				_stateValues.at(1) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(2)) {
				_stateValues.at(2) += 1;
			} else if (_nStates > 3) {
				if (_samples.at(counter) < _stateIntervals.at(3)) {
					_stateValues.at(3) += 1;
				} else {
					_stateValues.at(4) += 1;
				}
			}
			counter++;
		}

		_stateValues.at(0) = (double) _stateValues.at(0) / EQUIDISTANT_SAMPLESIZE;
		if(_nStates > 1)
			_stateValues.at(1) = (double) _stateValues.at(1) / EQUIDISTANT_SAMPLESIZE;
		if(_nStates > 2)
			_stateValues.at(2) = (double) _stateValues.at(2) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 3)
			_stateValues.at(3) = (double) _stateValues.at(3) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 4)
			_stateValues.at(4) = (double) _stateValues.at(4) / EQUIDISTANT_SAMPLESIZE;
	
		_tpn.push_back(_stateValues.at(0));
		if(_nStates > 1)
			_tpn.push_back(_stateValues.at(1));
		if(_nStates > 2)
			_tpn.push_back(_stateValues.at(2));
		if (_nStates > 3)
			_tpn.push_back(_stateValues.at(3));
		if (_nStates > 4)
			_tpn.push_back(_stateValues.at(4));
	}

	// @deprecated
	double RNode::weigthedMin()
	{
		double highestValue;
		double div = 0;
		double sum = 0;
		double aux = 0;
		double min = 123456789;
		double a = 0;
		double currentMin = 0;
		int seed = 123456789;

		for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
			for (int j = 0; j < _parents.size(); j++) {
				div = _parentsWeight.at(j) + _parents.size() - 1;
				a = _parents.at(j)->_samples.at(i) * _parentsWeight.at(j); 
				for (int k = 0; k < _parents.size(); k++) {
					if (k != j) {
						sum += _parents.at(k)->_samples.at(i);
					}
				}
			
				currentMin = (a + sum) / div;
				if (currentMin < min) {
					min = currentMin;
				}
				sum = 0; 			
			}
		
			aux = truncated_normal_ab_sample(min, _var, 0, 1, seed);

			int counter = 0;
			_samples.at(counter) = aux;

			if (_samples.at(counter) < _stateIntervals.at(0)) {
				_stateValues.at(0) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(1)) {
				_stateValues.at(1) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(2)) {
				_stateValues.at(2) += 1;
			} else if (_nStates > 3) {
				if (_samples.at(counter) < _stateIntervals.at(3)) {
					_stateValues.at(3) += 1;
				} else {
					_stateValues.at(4) += 1;
				}
			}
			counter++;
		}

		_stateValues.at(0) = (double) _stateValues.at(0) / EQUIDISTANT_SAMPLESIZE;		
		if(_nStates > 1)
			_stateValues.at(1) = (double) _stateValues.at(1) / EQUIDISTANT_SAMPLESIZE;
		if(_nStates > 2)
			_stateValues.at(2) = (double) _stateValues.at(2) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 3)
			_stateValues.at(3) = (double) _stateValues.at(3) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 4)
			_stateValues.at(4) = (double) _stateValues.at(4) / EQUIDISTANT_SAMPLESIZE;
	
		_tpn.push_back(_stateValues.at(0));
		if(_nStates > 1)
			_tpn.push_back(_stateValues.at(1));
		if(_nStates > 2)
			_tpn.push_back(_stateValues.at(2));
		if (_nStates > 3)
			_tpn.push_back(_stateValues.at(3));
		if (_nStates > 4)
			_tpn.push_back(_stateValues.at(4));
		return aux;
	}

	// @deprecated
	double RNode::weigthedMax()
	{
		double highestValue;
		double brierScore;
		double div = 0;
		double sum = 0;
		double aux = 0;
		double max = 0;
		double a = 0;
		double currentMax = 0;
		int seed = 123456789;

		for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
			for (int j = 0; j < _parents.size(); j++) {
				div = _parentsWeight.at(j) + _parents.size() - 1;
				a = _parents.at(j)->_samples.at(i) * _parentsWeight.at(j); 
				for (int k = 0; k < _parents.size(); k++) {
					if (k != j)
						sum += _parents.at(k)->_samples.at(i);
				}
			
				currentMax = (a + sum) / div;
				if (currentMax > max)
					max = currentMax;

				sum = 0; 
			}
		
			aux = truncated_normal_ab_sample(max, _var, 0, 1, seed);
			int counter = 0;
			_samples.at(counter) = aux;

			if (_samples.at(counter) < _stateIntervals.at(0)) {
				_stateValues.at(0) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(1)) {
				_stateValues.at(1) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(2)) {
				_stateValues.at(2) += 1;
			} else if (_nStates > 3) {
				if (_samples.at(counter) < _stateIntervals.at(3)) {
					_stateValues.at(3) += 1;
				} else {
					_stateValues.at(4) += 1;
				}
			}
			counter++;
		}

		_stateValues.at(0) = (double) _stateValues.at(0) / EQUIDISTANT_SAMPLESIZE;

		if(_nStates > 1)
			_stateValues.at(1) = (double) _stateValues.at(1) / EQUIDISTANT_SAMPLESIZE;
		if(_nStates > 2)
			_stateValues.at(2) = (double) _stateValues.at(2) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 3)
			_stateValues.at(3) = (double) _stateValues.at(3) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 4)
			_stateValues.at(4) = (double) _stateValues.at(4) / EQUIDISTANT_SAMPLESIZE;
	
		_tpn.push_back(_stateValues.at(0));
		if(_nStates > 1)
			_tpn.push_back(_stateValues.at(1));
		if(_nStates > 2)
			_tpn.push_back(_stateValues.at(2));
		if (_nStates > 3)
			_tpn.push_back(_stateValues.at(3));
		if (_nStates > 4)
			_tpn.push_back(_stateValues.at(4));

		return aux;
	}
	
	// @deprecated 
	// Wrong logic also
	void RNode::mixMinMax()
	{
		double sum = 0;
		double aux = 0;
		double max = 0;
		double min = 123456789;
		double a = 0;
		double w = 0;
		double wminFunction = 0;
		double div = 0;
		double currentMin = 0;
		double currentMax = 0;
		double mixMinMax = 0;
		int seed = 123456789;

		for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS) {
			for (int j = 0; j < _parents.size(); j++) {
				div = _parentsWeight.at(j) + _parents.size() - 1;
				a = _parents.at(j)->_samples.at(i) * _parentsWeight.at(j); 
				for (int k = 0; k < _parents.size(); k++) {
					if (k != j)
						sum += _parents.at(k)->_samples.at(i);
				}
			
				currentMin = (a + sum) / div;
				if (currentMin < min)
					min = currentMin;

				currentMax = (a + sum) / div;
				if (currentMax > max)
					max = currentMax;

				sum = 0; 
			}

			mixMinMax = (_wminValue * min + _wmaxValue * max) / (_wminValue + _wmaxValue);

			aux = truncated_normal_ab_sample(mixMinMax, _var, 0, 1, seed);

			int counter = 0;
			_samples.at(counter) = aux;

			if (_samples.at(counter) < _stateIntervals.at(0)) {
				_stateValues.at(0) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(1)) {
				_stateValues.at(1) += 1;
			} else if (_samples.at(counter) < _stateIntervals.at(2)) {
				_stateValues.at(2) += 1;
			} else if (_nStates > 3) {
				if (_samples.at(counter) < _stateIntervals.at(3)) {
					_stateValues.at(3) += 1;
				} else {
					_stateValues.at(4) += 1;
				}
			}
			counter++;
		}

		_stateValues.at(0) = (double) _stateValues.at(0) / EQUIDISTANT_SAMPLESIZE;
	
		if(_nStates > 1)
			_stateValues.at(1) = (double) _stateValues.at(1) / EQUIDISTANT_SAMPLESIZE;
		if(_nStates > 2)
			_stateValues.at(2) = (double) _stateValues.at(2) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 3)
			_stateValues.at(3) = (double) _stateValues.at(3) / EQUIDISTANT_SAMPLESIZE;
		if (_nStates > 4)
			_stateValues.at(4) = (double) _stateValues.at(4) / EQUIDISTANT_SAMPLESIZE;
	
		_tpn.push_back(_stateValues.at(0));
		if(_nStates > 1)
			_tpn.push_back(_stateValues.at(1));
		if(_nStates > 2)
			_tpn.push_back(_stateValues.at(2));
		if (_nStates > 3)
			_tpn.push_back(_stateValues.at(3));
		if (_nStates > 4)
			_tpn.push_back(_stateValues.at(4));
	}

	void RNode::removeParent(RNode *parent)
	{
		for (int i = 0; i < _parents.size(); i++) {
			if (_parents.at(i) == parent) {
				_parents.at(i)->removeChild(this);
				_parents.erase(_parents.begin()+i);
				_parentsWeight.erase(_parentsWeight.begin()+i);
			}
		}
	}

	void RNode::removeParentOnly(RNode *parent)
	{
		for(int i = 0; i < _parents.size(); i++) {
			if(_parents.at(i) == parent) {
				_parents.erase(_parents.begin()+i);
				_parentsWeight.erase(_parentsWeight.begin()+i);
				break;
			}
		}
	}

	void RNode::removeChild(RNode *child)
	{
		for(int i = 0; i < _childNodes.size(); i++)
		{
			if(_childNodes.at(i) == child)
			{
				_childNodes.erase(_childNodes.begin()+i);
			
				clog << ">>> INFO: Child removed from parent.";
				break;
			}
		}
	}

	void RNode::fillSmileCPT(DSL_sysCoordinates *coordinates, int parentIndex)
	{
		double tMean;
		for (int i = 0; i < _parents.at(parentIndex)->_nStates; i++) {
			if(_parents.at(parentIndex)->_nStates == 1) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .500000;
			} else if (_parents.at(parentIndex)->_nStates == 2) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .250000;
			} else if (_parents.at(parentIndex)->_nStates == 3) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .166667;
			} else if (_parents.at(parentIndex)->_nStates == 4) {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .125000;
			} else {
				tMean = (double) _parents.at(parentIndex)->_stateIntervals.at(i) - .1;
			}
		
			vector<double> tSamples = SamplesRepository::reuseBasicSample(tMean, _parents.at(parentIndex)->_nStates);
			if (tSamples.size() == 0) {
				_parents.at(parentIndex)->generateEvidence(tMean);
				SamplesRepository::addBasicSample(tMean, _parents.at(parentIndex)->_nStates, _parents.at(parentIndex)->_samples);
			} else {
				_parents.at(parentIndex)->_samples = tSamples;
			}

			if ((parentIndex + 1) == _parents.size()) {
				_sampleMixer->mix(this);
			
				for (int z = 0; z < _nStates; z++) {
					coordinates->UncheckedValue() = _stateValues.at(z);
					coordinates->Next();
				}

			} else {
				fillSmileCPT(coordinates, parentIndex + 1);
			}
		}
	}

	void RNode::setState(int pos, string name)
	{
		if (pos < _stateTitles.size()) {
			_stateTitles.at(pos) = name;
		} else {
			_stateTitles.push_back(name);
			_nStates = _stateTitles.size();
		
			_stateIntervals.clear();
			for (int i = 1; i <= _nStates; i++) {
				_stateIntervals.push_back((double) i / _nStates);
			}
		}
	}

	void RNode::setEvidence(short evidence)
	{
		_evidence = evidence;
	}

	short RNode::getEvidence()
	{
		return _evidence;
	}

	int RNode::getId()
	{
		return _id;
	}

	void RNode::fillSmileFileCPTValues(DSL_sysCoordinates *coordinates)
	{
		for(int i = 0; i < _tpn.size(); i++) {
			coordinates->UncheckedValue() = _tpn.at(i);
			coordinates->Next();
		}
	}

	void RNode::addParentId(int id)
	{
		_parentsId.push_back(id);
	}

	void RNode::addChildId(int id)
	{
		_childrenId.push_back(id);
	}

	int RNode::getParentId(int parentIndex)
	{
		return _parentsId.at(parentIndex);
	}

	int RNode::getChildId(int childIndex)
	{
		return _childrenId.at(childIndex);
	}

	vector<int> RNode::getParentsId()
	{
		return _parentsId;
	}

	/* 
	** Preenche TPN com zeros.
	*/
	void RNode::fillEmptyNPT(int parentIndex) {
		if (parentIndex == 0)
			_tpn.clear();
		for (int i = 0; i < _parents.at(parentIndex)->_nStates; i++) {
			if(parentIndex + 1 == size()) {
				for (int j = 0; j < _nStates; j++)
					_tpn.push_back(0);
			} else {
				fillEmptyNPT(parentIndex + 1);
			}
		}
	}

	void RNode::numberOfStatesChanged(){
		for (int i = 0; i < _childNodes.size(); i++) {
			_childNodes.at(i)->setNptNeedUpdate(true);
		}
		setNptNeedUpdate(true);
	};

	void RNode::notifyChildOnDelete(){
		for (int i = 0; i < _childNodes.size(); i++) {
			_childNodes.at(i)->setNptNeedUpdate(true);
		}
	};

	void RNode::setNptNeedUpdate(bool need) {
		_manualNptneedUpdate = need;
	};

	bool RNode::manualNptNeedUpdate() {
		return _manualNptneedUpdate;
	}

	void RNode::setWMaxValue(double wmax) {
		_wmaxValue = wmax;
	}

	void RNode::setWMinValue(double wmin) {
		_wminValue = wmin;
	}

	double RNode::getWMinValue() {
		return _wminValue;
	}

	double RNode::getWMaxValue() {
		return _wmaxValue;
	}

	void RNode::addParent(RNode* parentNode, double weight)
	{
		setParent(parentNode);
		setParentWeight(weight);
		parentNode->setChild(this);
	}

	void RNode::setChild(RNode* childNode)
	{
		_childNodes.push_back(childNode);
	}

	void RNode::setParent(RNode* parentNode)
	{
		_parents.push_back(parentNode);
	}

	void RNode::setParentWeight(double weight)
	{
		_parentsWeight.push_back(weight);
	}

	void RNode::setMean(double inMu)
	{
		_mu = inMu;
	}

	void RNode::setVariance(double inVar)
	{
		_var = inVar;
	}

	void RNode::setTpn(vector<double> tpn)
	{
		_tpn = tpn;
	}

	void RNode::setSamples(vector<double> samples)
	{
		_samples = samples;
	}

	void RNode::setName(string name)
	{
		_name = name;
	}

	bool RNode::isChild()
	{
		return _parents.size() > 0;	
	}

	vector<double>* RNode::getTpnPtr()
	{
		return &_tpn;
	}

	vector<string>* RNode::getStateTitlesPtr()
	{
		return &_stateTitles;
	}

	vector<RNode*>* RNode::getParentsPtr()
	{
		return &_parents;
	}

	vector<double>* RNode::getStateValuesPtr()
	{
		return &_stateValues;
	}

	vector<double>* RNode::getStateIntervalsPtr()
	{
		return &_stateIntervals;
	}

	vector<double>* RNode::getSamplesPtr()
	{
		return &_samples;
	}

	int RNode::getNumberOfStates()
	{
		return _nStates;
	}

	double RNode::getVariance()
	{
		return _var;
	}

	int RNode::size()
	{
		return _parents.size();
	}

	vector<double>* RNode::getParentsWeightPtr()
	{
		return &_parentsWeight;
	}

	string RNode::getName()
	{
		return _name;
	}

	vector<RNode*> *RNode::getChildsPtr()
	{
		return &_childNodes;
	}

	vector<RNode*> RNode::getParents()
	{
		return _parents;
	}

	RNode *RNode::withState(string name)
	{
		if(_stateTitles.size() <= MAX_NUMBER_OF_STATES)
			_stateTitles.push_back(name);
		return this;
	}

	RNode *RNode::withMean(double inMu)
	{
		_mu = inMu;
		return this;
	}

	RNode *RNode::withVariance(double inVar)
	{
		_var = inVar;
		return this;
	}

	RNode *RNode::withName(string nodeName)
	{
		_name = nodeName;
		return this;
	}

	void RNode::setSampleMixer(SampleMixer *mixer)
	{
		delete _sampleMixer;
		_sampleMixer = mixer;
	}

	void RNode::setSampleMixer()
	{
		delete _sampleMixer;
		switch (wExpression)
		{
		case KaizenLibrary::WMEAN:
			_sampleMixer = new WMean;
			break;
		case KaizenLibrary::WMIN:
			_sampleMixer = new WMin;
			break;
		case KaizenLibrary::WMAX:
			_sampleMixer = new WMax;
			break;
		case KaizenLibrary::MIXMINMAX:
			_sampleMixer = new MixMinMax;
			break;
		default:
			_sampleMixer = new WMean;
			break;
		}
		
	}

	void RNode::wmean()
	{
		clog << ">>> Entrou em WMEAN";
		double div = 0;
		double sum = 0;
		double mean = 0;
		double aux = 0;
		int seed = 123456789;

		for (int i = 0; i < _parents.size(); i++){
			div += _parentsWeight.at(i);
		}

		int counter = 0;
	
		cleanStateValues();
		for (int i = 0; i < SAMPLESIZE; i+=EQUIDISTANT_PASS)
		{
			for (int j = 0; j < _parents.size(); j++)
			{
				sum += _parents.at(j)->getSamplesPtr()->at(i) * _parentsWeight.at(j);
			}
		
			mean = sum / div;
		
			sum = 0; 

			aux = truncated_normal_ab_sample(mean, _var, 0,
			1, seed);

			_samples.at(counter) = aux;

			if (_samples.at(counter) < _stateIntervals.at(0))
			{
				_stateValues.at(0) += 1;
			}
			else if (_samples.at(counter) < _stateIntervals.at(1))
			{
				_stateValues.at(1) += 1;
			}
			else if (_samples.at(counter) < _stateIntervals.at(2))
			{
				_stateValues.at(2) += 1;
			}
			else if (_nStates > 3)
			{
				if (_samples.at(counter) < _stateIntervals.at(3))
				{
					_stateValues.at(3) += 1;
				}
				else
				{
					_stateValues.at(4) += 1;
				}
			}
			counter++;
		}

		_stateValues.at(0) = (double) _stateValues.at(0) / EQUIDISTANT_SAMPLESIZE;
	
		if(_nStates > 1)
			_stateValues.at(1) = (double) _stateValues.at(1) / EQUIDISTANT_SAMPLESIZE;
	
		if(_nStates > 2)
			_stateValues.at(2) = (double) _stateValues.at(2) / EQUIDISTANT_SAMPLESIZE;

		if (_nStates > 3)
			_stateValues.at(3) = (double) _stateValues.at(3) / EQUIDISTANT_SAMPLESIZE;

		if (_nStates > 4)
			_stateValues.at(4) = (double) _stateValues.at(4) / EQUIDISTANT_SAMPLESIZE;

		_tpn.push_back(_stateValues.at(0));

		if(_nStates > 1)
			_tpn.push_back(_stateValues.at(1));
	
		if(_nStates > 2)
			_tpn.push_back(_stateValues.at(2));

		if (_nStates > 3)
			_tpn.push_back(_stateValues.at(3));

		if (_nStates > 4)
			_tpn.push_back(_stateValues.at(4));
	}

	void RNode::cleanStateValues()
	{
		for (int i = 0; i < _nStates; i++) {
			_stateValues.at(i) = 0;
		}
	}

	void RNode::notifyChanges() 
	{
		cerr << "Yet to be implemented" << endl;
	}

	void RNode::setId(int id)
	{
		this->_id = id;
	}

	void RNode::setFunction(std::string function)
	{
		if (function == "none" || function == "manual") {
			wExpression = NONE;
		} else if (function == "wmin") {
			wExpression = WMIN;
		} else if (function == "wmax") {
			wExpression = WMAX;
		} else if (function == "mixminmax") {
			wExpression = MIXMINMAX;
		} else {
			wExpression = WMEAN;
		}

		// Quando sem argumentos, define o sample baseado na wExpression
		setSampleMixer();

	}

	void RNode::addNPTEntry(double entry) 
	{
		_tpn.push_back(entry);
	}

}