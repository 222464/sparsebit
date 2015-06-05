#include "SDRT.h"
#include <iostream>
#include <math.h>

void SDRT::create(int inputWidth, int inputHeight, int width, int height,
	float connectionRadius, float inhibitionRadius,
	std::mt19937 &generator, bool predict)
{
	std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

	_inputWidth = inputWidth;
	_inputHeight = inputHeight;

	_width = width;
	_height = height;

	_connectionRadius = connectionRadius;
	_inhibitionRadius = inhibitionRadius;

	_nodes.resize(width * height);

	float avgConnections = 2.0f * std::_Pi * std::pow(connectionRadius + 0.5f, 2);
	float avgInhibitionConnections = 2.0f * std::_Pi * std::pow(inhibitionRadius + 0.5f, 2);

	float nodeToInputWidth = static_cast<float>(inputWidth + 1) / static_cast<float>(height + 1);
	float nodeToInputHeight = static_cast<float>(inputHeight + 1) / static_cast<float>(height + 1);

	for (int i = 0; i < _nodes.size(); i++)
	{
		int x = i % width;
		int y = i / width;

		int inputX = std::round((x + 0.5f) * nodeToInputWidth);
		int inputY = std::round((y + 0.5f) * nodeToInputHeight);

		_nodes[i]._inhibitionBias = 0.0f;

		_nodes[i]._connections.reserve(avgConnections);
		_nodes[i]._inhibitionConnections.reserve(avgInhibitionConnections);

		for (int ox = inputX - connectionRadius; ox <= inputX + connectionRadius; ox++)
			for (int oy = inputY - connectionRadius; oy <= inputY + connectionRadius; oy++)
				if (ox >= 0 && ox < inputWidth && oy >= 0 && oy < inputHeight)
					if (std::sqrt(static_cast<float>(std::pow(inputX - ox, 2) + std::pow(inputY - oy, 2))) <= connectionRadius)
						_nodes[i]._connections.push_back(Connection{ ox + oy * inputWidth, dist01(generator), 1.0f, false });

		if (predict)
			for (int ox = x - connectionRadius; ox <= x + connectionRadius; ox++)
				for (int oy = x - connectionRadius; oy <= x + connectionRadius; oy++)
					if (ox >= 0 && ox < width && oy >= 0 && oy < height)
						if (std::sqrt(static_cast<float>(std::pow(x - ox, 2) + std::pow(y - oy, 2))) <= connectionRadius)
							_nodes[i]._connections.push_back(Connection{ ox + oy * width, dist01(generator), 1.0f, true });

		for (int ox = x - inhibitionRadius; ox <= x + inhibitionRadius; ox++)
			for (int oy = y - inhibitionRadius; oy <= y + inhibitionRadius; oy++)
				if (ox >= 0 && ox < width && oy >= 0 && oy < height)
					if (std::sqrt(static_cast<float>(std::pow(x - ox, 2) + std::pow(y - oy, 2))) <= inhibitionRadius)
						_nodes[i]._inhibitionConnections.push_back(Connection{ ox + oy * width, dist01(generator), std::max(0.0f, 1.0f - std::sqrt(static_cast<float>(std::pow(x - ox, 2) + std::pow(y - oy, 2))) / inhibitionRadius), true });

		_nodes[i]._connections.shrink_to_fit();
		_nodes[i]._inhibitionConnections.shrink_to_fit();
	}
}

float sigmoid(float x)
{
	return (1.0f / (1.0f + std::exp(-x)));
}

SDR SDRT::generateSDR(const std::vector<float> inputs)
{
	std::vector<float> _states(_width * _height);

	for (int i = 0; i < _nodes.size(); i++)
	{
		float sum = 0.0f;

		for (int j = 0; j < _nodes[i]._connections.size(); j++)
			if (_nodes[i]._connections[j]._selfConnection)
				sum += std::pow(_nodes[_nodes[i]._connections[j]._inputIndex]._state - _nodes[i]._connections[j]._weight, 2);
			else
				sum += std::pow(inputs[_nodes[i]._connections[j]._inputIndex] - _nodes[i]._connections[j]._weight, 2);

		_nodes[i]._activation = -sum;
	}	
	
	for (int i = 0; i < _nodes.size(); i++)
	{
		int nodeX = i % _width;
		int nodeY = i / _width;
		
		float inhibitionSum = 0.0f;

		for (int j = 0; j < _nodes[i]._inhibitionConnections.size(); j++)
			if (_nodes[i]._activation < _nodes[_nodes[i]._inhibitionConnections[j]._inputIndex]._activation)
				inhibitionSum += _nodes[i]._inhibitionConnections[j]._weight * _nodes[i]._inhibitionConnections[j]._fallOff;

		if ((1.0f - inhibitionSum * sigmoid(_nodes[i]._inhibitionBias)) > 0.0f)
			_nodes[i]._state = 1.0f;
		else
			_nodes[i]._state = 0.0f;
	}

	for (int i = 0; i < _nodes.size(); i++)
		_states[i] = _nodes[i]._state;

	return SDR{ _width, _height, _states };
}

std::vector<float> SDRT::reconstruct(const SDR &sdr)
{
	std::vector<float> data(_inputWidth * _inputHeight, 0.0f);
	std::vector<float> stateSums(_inputWidth * _inputHeight , 0.0f);

	for (int i = 0; i < _nodes.size(); i++)
	{
		for (int j = 0; j < _nodes[i]._connections.size(); j++)
		{
			if (!_nodes[i]._connections[j]._selfConnection)
			{
				data[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state * _nodes[i]._connections[j]._weight;
				stateSums[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state;
			}
		}
	}

	for (int i = 0; i < data.size(); i++)
	{
		if (stateSums[i] > 0.0f)
			data[i] /= stateSums[i];
		else
			data[i] = 0.0f;
	}

	return data;
}

std::vector<float> SDRT::reconstructPrediction(const SDR &sdr)
{
	std::vector<float> data(_width * _height, 0.0f);
	std::vector<float> stateSums(_width * _height, 0.0f);

	for (int i = 0; i < _nodes.size(); i++)
	{
		for (int j = 0; j < _nodes[i]._connections.size(); j++)
		{
			if (_nodes[i]._connections[j]._selfConnection)
			{
				data[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state * _nodes[i]._connections[j]._weight;
				stateSums[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state;
			}
		}
	}

	for (int i = 0; i < data.size(); i++)
	{
		if (stateSums[i] > 0.0f)
			data[i] /= stateSums[i];
		else
			data[i] = 0.0f;
	}

	return data;
}

void SDRT::learn(const std::vector<float> &inputs, const SDR &sdr, float weightLearnRate, float inhibitionLearnRate, float inhibitionBiasLearnRate, float sparsity)
{
	std::vector<float> inputRecon = reconstruct(sdr);
	std::vector<float> predictionRecon = reconstructPrediction(sdr);
	std::vector<float> errors(_inputWidth * _inputHeight);
	std::vector<float> predictionErrors(_width * _height);

	float totalError = 0.0f;

	for (int i = 0; i < inputs.size(); i++) 
	{
		errors[i] = inputs[i] - inputRecon[i];
		totalError += std::pow(errors[i], 2);
	}

	for (int i = 0; i < sdr._states.size(); i++)
	{
		predictionErrors[i] = sdr._states[i] - predictionRecon[i];
		totalError += std::pow(predictionErrors[i], 2);
	}

	std::cout << "Total Error: " << totalError << std::endl;

	for (int i = 0; i < _nodes.size(); i++)
	{
		_nodes[i]._inhibitionBias += (_nodes[i]._state - sparsity) * inhibitionBiasLearnRate;

		for (int j = 0; j < _nodes[i]._inhibitionConnections.size(); j++)
		{
			float inhibition = _nodes[i]._activation > _nodes[_nodes[i]._inhibitionConnections[j]._inputIndex]._activation ? 1.0f : 0.0f;
			_nodes[i]._inhibitionConnections[j]._weight = std::max<float>(0.0f, _nodes[i]._inhibitionConnections[j]._weight + (_nodes[i]._state * inhibition - sparsity * sparsity) * inhibitionLearnRate);
		}

		for (int j = 0; j < _nodes[i]._connections.size(); j++)
		{
			if (!_nodes[i]._connections[j]._selfConnection)
				_nodes[i]._connections[j]._weight += _nodes[i]._state * errors[_nodes[i]._connections[j]._inputIndex] * weightLearnRate;
			else
				_nodes[i]._connections[j]._weight += _nodes[i]._state * predictionErrors[_nodes[i]._connections[j]._inputIndex] * weightLearnRate;
		}
	}
}
