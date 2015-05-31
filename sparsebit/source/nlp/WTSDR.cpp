#include "WTSDR.h"
#include <unordered_map>

void nlp::WTSDR::create(int inputWidth, int inputHeight, int width, int height, float connectionRadius, float inhibitionRadius, float inhibitionThreshold, std::mt19937 &generator)
{
	_sdrt.create(inputWidth, inputHeight, width, height, connectionRadius, inhibitionRadius, inhibitionThreshold, generator);
}

SDR nlp::WTSDR::convertStringToSDR(const std::string &word, const std::string &definition)
{
	std::vector<std::string> words;
	std::vector<float> inputs;

	std::string sub = "";

	for (int i = 0; i < definition.length(); i++)
	{
		if (definition[i] == ' ' || definition[i] == '.' || definition[i] == '!' || definition[i] == '?' || definition[i] == ',' || definition[i] == ';' || definition[i] == '-')
		{
			if (!sub.empty())
				words.push_back(sub);

			continue;
		}

		sub += definition[i];
	}

	SDR wordDefSDR;

	for (int i = 0; i < words.size(); i++)
	{
		inputs.clear();

		for (int j = 0; j < words[i].length(); j++)
		{
			inputs.push_back(static_cast<float>(words[i][j]));
		}

		wordDefSDR = _sdrt.generateSDR(inputs);
	}

	inputs.clear();

	for (int i = 0; i < word.length(); i++)
	{
		inputs.push_back(static_cast<float>(word[i]));
	}

	SDR wordSDR = _sdrt.generateSDR(inputs);

	return wordSDR;
}


