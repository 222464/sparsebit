#include "WTSDR.h"
#include <iostream>

void nlp::WTSDR::create(int inputWidth, int inputHeight, int width, int height, float connectionRadius, float inhibitionRadius, std::mt19937 &generator)
{
	_inputWidth = inputWidth;
	_inputHeight = inputHeight;

	_sdrt.create(_inputWidth, _inputHeight, width, height, connectionRadius, inhibitionRadius, generator, true);
}

void nlp::WTSDR::emplaceWord(const std::string &word)
{
	bool contains = false;

	for (int i = 0; i < _words.size(); i++)
		if (_words[i] == word)
		{
			contains = true;
			break;
		}

	if (!contains)
		_words.push_back(word);
}

std::vector<float> nlp::WTSDR::getVectorFromWords(const std::vector<std::string> &words)
{
	std::vector<float> vec(_inputWidth * _inputHeight, 0.0f);

	for (int i = 0; i < words.size(); i++)
	{
		for (int j = 0; i < _words.size(); j++)
		{
			if (_words[j] == words[i])
			{
				vec[j] = 1.0f;
				break;
			}
		}
	}

	return vec;
}

SDR nlp::WTSDR::learn(const std::vector<std::string> &words, float weightLearnRate, float inhibitionLearnRate, float inhibitionBiasLearnRate, float sparsity, int numLearnSteps)
{
	for (int i = 0; i < words.size(); i++)
		emplaceWord(words[i]);

	std::vector<float> inputs = getVectorFromWords(words);

	SDR sdr;
	
	for (int i = 0; i < numLearnSteps; i++)
	{
		sdr = _sdrt.generateSDR(inputs);

		_sdrt.learn(inputs, sdr, weightLearnRate, inhibitionLearnRate, inhibitionBiasLearnRate, sparsity);
	}

	return sdr;
}

std::string nlp::WTSDR::getStringFromSDR(const SDR &sdr, float threshold)
{
	std::string str = "";

	std::vector<float> inputs = _sdrt.reconstruct(sdr);

	for (int i = 0; i < inputs.size(); i++)
		if (inputs[i] > threshold)
			str += _words[i] + " ";

	return str;
}