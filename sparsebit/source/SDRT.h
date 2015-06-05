#pragma once

#include <vector>
#include <random>
#include <algorithm>

#include "SDR.h"

class SDRT
{
private:
	struct Connection
	{
		int _inputIndex;
		float _weight;
		float _fallOff;
		bool _selfConnection;
	};

	struct Node
	{
		std::vector<Connection> _connections;
		std::vector<Connection> _inhibitionConnections;
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

	std::vector<Node> _nodes;

public:
	void create(int inputWidth, int inputHeight, int width, int height,
		float connectionRadius, float inhibitionRadius,
		std::mt19937 &generator, bool predict = false);

	SDR generateSDR(const std::vector<float> inputs);

	std::vector<float> reconstruct(const SDR &sdr);
	std::vector<float> SDRT::reconstructPrediction(const SDR &sdr);

	void learn(const std::vector<float> &inputs, const SDR &sdr, float weightLearnRate, float inhibitionLearnRate, float inhibitionBiasLearnRate, float sparsity);

	float getState(int index) const
	{
		return _nodes[index]._state;
	}

	float getState(int x, int y) const
	{
		return getState(x + y * _width);
	}
};