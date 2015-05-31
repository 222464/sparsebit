#include "SDRT.h"
#include <iostream>
#include <math.h>

void SDRT::create(int inputWidth, int inputHeight, int width, int height,
	float connectionRadius, float inhibitionRadius, float inhibitionThreshold,
	std::mt19937 &generator)
{
	std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

	_inputWidth = inputWidth;
	_inputHeight = inputHeight;

	_width = width;
	_height = height;

	_connectionRadius = connectionRadius;
	_inhibitionRadius = inhibitionRadius;

	_inhibitionThreshold = inhibitionThreshold;

	_nodes.resize(width * height);

	float avgConnections = 2.0f * std::_Pi * std::pow(connectionRadius + 0.5f, 2);

	float nodeToInputWidth = static_cast<float>(inputWidth + 1) / static_cast<float>(height + 1);
	float nodeToInputHeight = static_cast<float>(inputHeight + 1) / static_cast<float>(height + 1);

	for (int i = 0; i < _nodes.size(); i++)
	{
		int x = i % width;
		int y = i / width;

		int inputX = std::round((x + 0.5f) * nodeToInputWidth);
		int inputY = std::round((y + 0.5f) * nodeToInputHeight);

		_nodes[i]._numInhibitionConnections = 0.0f;
		_nodes[i]._inhibitionBias = 0.0f;
		_nodes[i]._connections.reserve(avgConnections);

		for (int ox = inputX - connectionRadius; ox <= inputX + connectionRadius; ox++)
			for (int oy = inputY - connectionRadius; oy <= inputY + connectionRadius; oy++)
				if (ox >= 0 && ox < inputWidth && oy >= 0 && oy < inputHeight)
				{
					if (std::sqrt(static_cast<float>(std::pow(inputX - ox, 2) + std::pow(inputY - oy, 2))) <= connectionRadius)
					{
						_nodes[i]._connections.push_back(Connection{ ox + oy * inputWidth, dist01(generator) });
						_nodes[i]._numInhibitionConnections++;
					}
				}
				else if (std::sqrt(static_cast<float>(std::pow(inputX - ox, 2) + std::pow(inputY - oy, 2))) <= connectionRadius)
					_nodes[i]._numInhibitionConnections++;

		for (int ox = x - connectionRadius; ox <= x + connectionRadius; ox++)
			for (int oy = y - connectionRadius; oy <= y + connectionRadius; oy++)
				if (ox >= 0 && ox < width && oy >= 0 && oy < height)
				{
					if (std::sqrt(static_cast<float>(std::pow(x - ox, 2) + std::pow(y - oy, 2))) <= connectionRadius)
					{
						_nodes[i]._recurrentConnections.push_back(Connection{ ox + oy * width, dist01(generator) });
						_nodes[i]._numRecurrentConnections++;
					}
				}
				else if (std::sqrt(static_cast<float>(std::pow(x - ox, 2) + std::pow(y - oy, 2))) <= connectionRadius)
					_nodes[i]._numRecurrentConnections++;

		_nodes[i]._connections.shrink_to_fit();
	}
}

SDR SDRT::generateSDR(const std::vector<float> inputs)
{
	std::vector<float> _states(_width * _height);

	for (int i = 0; i < _nodes.size(); i++)
	{
		float sum = 0.0f;

		for (int j = 0; j < _nodes[i]._connections.size(); j++)
			sum += std::pow(inputs[_nodes[i]._connections[j]._inputIndex] - _nodes[i]._connections[j]._weight, 2);

		if (recurrentConnections)
			for (int j = 0; j < _nodes[i]._recurrentConnections.size(); j++)
				sum += std::pow(_nodes[_nodes[i]._recurrentConnections[j]._inputIndex]._state - _nodes[i]._recurrentConnections[j]._weight, 2);

		_nodes[i]._activation = -sum + _nodes[i]._inhibitionBias;
	}
	
	for (int i = 0; i < _nodes.size(); i++)
	{
		int nodeX = i % _width;
		int nodeY = i / _width;
		
		float inhibitionCounter = 0;

		for (int ox = nodeX - _inhibitionRadius; ox <= nodeX + _inhibitionRadius; ox++)
			for (int oy = nodeY - _inhibitionRadius; oy <= nodeY + _inhibitionRadius; oy++)
				if (ox >= 0 && ox < _width && oy >= 0 && oy < _height)
				{
					if (std::sqrt(static_cast<float>(std::pow(ox - nodeX, 2) + std::pow(oy - nodeY, 2))) <= _inhibitionRadius)
						if (_nodes[i]._activation < _nodes[ox + oy * _width]._activation)
							inhibitionCounter++;
				}
				else if (std::sqrt(static_cast<float>(std::pow(ox - nodeX, 2) + std::pow(oy - nodeY, 2))) <= _inhibitionRadius)
					inhibitionCounter += _inhibitionThreshold / (_nodes[i]._numInhibitionConnections + _nodes[i]._numRecurrentConnections);

		if (inhibitionCounter < _inhibitionThreshold)
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
			data[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state * _nodes[i]._connections[j]._weight;
			stateSums[_nodes[i]._connections[j]._inputIndex] += _nodes[i]._state;
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

void SDRT::learn(const std::vector<float> &inputs, const SDR &sdr, float weightLearnRate, float inhibitionLearnRate, float recurrentLearnRate)
{
	std::vector<float> inputRecon = reconstruct(sdr);
	std::vector<float> errors(_inputWidth * _inputHeight);
	std::vector<float> recurrentErrors(_width * _height);
	float totalError = 0.0f;

	for (int i = 0; i < inputs.size(); i++) 
	{
		errors[i] = inputs[i] - inputRecon[i];
		totalError += std::pow(errors[i], 2);
	}

	std::cout << totalError << std::endl;

	for (int i = 0; i < _nodes.size(); i++)
	{
		float errorSum = 0.0f;
		for (int j = 0; j < _nodes[i]._connections.size(); j++)
			errorSum += std::pow(inputs[_nodes[i]._connections[j]._inputIndex] - _nodes[i]._connections[j]._weight, 2);
	}

	for (int i = 0; i < _nodes.size(); i++)
	{
		for (int j = 0; j < _nodes[i]._connections.size(); j++)
		{
			_nodes[i]._connections[j]._weight += _nodes[i]._state * errors[_nodes[i]._connections[j]._inputIndex] * weightLearnRate;
		}

		_nodes[i]._inhibitionBias += (_nodes[i]._state - _inhibitionThreshold / _nodes[i]._numInhibitionConnections) * inhibitionLearnRate;

		for (int j = 0; j < _nodes[i]._recurrentConnections.size(); j++)
		{
			_nodes[i]._recurrentConnections[j]._weight += _nodes[i]._state * recurrentErrors[_nodes[i]._recurrentConnections[j]._inputIndex] * recurrentLearnRate;
		}
	}
}
