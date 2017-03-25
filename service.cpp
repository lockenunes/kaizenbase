#pragma once
#include "service.h"
#include <iostream>
#include "include\rapidjson\document.h"
#include "include\rapidjson\writer.h"
#include "include\rapidjson\stringbuffer.h"
#include "b_network.h"
#include "r_node.h"
#include <string>

using namespace rapidjson;

namespace KaizenLibrary
{
   std::string Services::fillNPT(char* json)
   {
   	Document r;
	r.SetObject();
	Document d;
	d.Parse(json);

	Value& net = d["network"];
	Value& nodes = net["nodes"];
	BNetwork *network = new BNetwork();

	network->setNetworkName("network");

	int temp_id = 0;
	int nStates = 0;

	for (SizeType i = 0; i < nodes.Size(); i++) {
		Value& nodex = nodes[i];
		try {
			if (!nodex.HasMember("node_id") || !nodex.HasMember("states")
					|| !nodex["states"].IsArray()) {
				throw 1;
			}
		} catch (int e) {
			r.AddMember("status", "fail", r.GetAllocator());
			r.AddMember("message", "Invalid data", r.GetAllocator());

			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			r.Accept(writer);
			std::string result = buffer.GetString();
			return result;
		}

		Value& id = nodex["node_id"];
		Value& variance = nodex["variance"];
		Value& states = nodex["states"];
		Value& evidence = nodex["evidence"];
		Value& function = nodex["function"];

		temp_id = id.GetInt();
		nStates = states.GetArray().Size();

		RNode* node = network->createNode(temp_id, nStates);
		node->setVariance(variance.GetDouble());
		node->setEvidence(evidence.GetInt());
		node->setFunction(function.GetString());

		// Se não informar min e max, valores default serão utilizados
		if (node->wExpression == MIXMINMAX) {
			if (nodex.HasMember("min") && nodex.HasMember("max")) {
				Value& min = nodex["min"];
				Value& max = nodex["max"];
				node->setWMaxValue(max.GetDouble());
				node->setWMinValue(min.GetDouble());
			} else {
				// Default values
				node->setWMaxValue(5);
				node->setWMinValue(1);
			}
		} else if (node->wExpression == NONE) {
				// NPT manual
				Value& npt_tmp = nodex["npt"];
				for (SizeType i = 0; i < npt_tmp.Size(); i++) {
					node->addNPTEntry(npt_tmp[i].GetDouble());
				}
		}

	}
	// Crianco as relações
	for (SizeType i = 0; i < nodes.Size(); i++) {
		Value& nodex = nodes[i];
		Value& parents = nodex["parents"];
		for (SizeType j = 0; j < parents.GetArray().Size(); ++j) {
			Value& relation = parents[j];
			network->createRelation(nodex["node_id"].GetInt(),
					relation["node_id"].GetInt(),
					relation["weight"].GetDouble());
		}
	}

	network->calculateTPN();

	// Grava de volta no json
	for (SizeType i = 0; i < nodes.Size(); i++) {
		Value& nodex = nodes[i];
		Value& parents = nodex["parents"];
		Value& states = nodex["states"];
		RNode* node = network->getNodeById(nodex["node_id"].GetInt());

		for (int k = 0; k < states.Size(); ++k) {
			states[k] = node->getStateValuesPtr()->at(k);
		}

		if (parents.GetArray().Size() > 0 && node->wExpression != NONE) {
			Value& npt = nodex["npt"];
			npt.Clear();
			Document::AllocatorType& allocator = d.GetAllocator();
			for (int j = 0; j < node->getTpnPtr()->size(); ++j) {
				npt.PushBack(Value(node->getTpnPtr()->at(j)).Move(), allocator);
			}
		}
	}

	r.AddMember("status", "success", r.GetAllocator());
	r.AddMember("data", d, r.GetAllocator());

	// Gravando de volta
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	r.Accept(writer);

	std::string result = buffer.GetString();

	delete network;

	return result;
   }

}