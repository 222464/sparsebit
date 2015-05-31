#pragma once

#include <vector>
#include <random>

#include "SDR.h"

class SDRT
{
private:
	struct Connection
	{
		int _inputIndex;
		float _weight;
	};

	struct Node
	{
		std::vector<Connection> _connections;
		std::vector<Connection> _recurrentConnections;

		float _numInhibitionConnections;
		float _numRecurrentConnections;

		float _inhibitionBias;
		// before inhibition
		float _activation;
		// after inhibition
		float _state;
	};

	int _inputWidth;
	int _inputHeight;

	int _width;
	int _height;

	float _connectionRadius;
	float _inhibitionRadius;
	
	float _inhibitionThreshold;

	std::vector<Node> _nodes;

public:
	void create(int inputWidth, int inputHeight, int width, int height,
		float connectionRadius, float inhibitionRadius, float inhibitionThreshold,
		std::mt19937 &generator);

	SDR generateSDR(const std::vector<float> inputs, bool recurrentConnections = false);

	std::vector<float> reconstruct(const SDR &sdr);

	void learn(const std::vector<float> &inputs, const SDR &sdr, float weightLearnRate, float inhibitionLearnRate, float recurrentLearnRate = 0.0f);

	float getState(int index) const
	{
		return _nodes[index]._state;
	}

	float getState(int x, int y) const
	{
		return getState(x + y * _width);
	}
};