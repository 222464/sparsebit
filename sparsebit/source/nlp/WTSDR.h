#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "SDRT.h"

namespace nlp
{
	class WTSDR
	{
	private:
		int _inputWidth;
		int _inputHeight;

		std::vector<std::string> _words;

		void emplaceWord(const std::string &word);

	public:
		SDRT _sdrt;

		void create(int inputWidth, int inputHeight, int width, int height, float connectionRadius, float inhibitionRadius, std::mt19937 &generator);

		std::vector<float> getVectorFromWords(const std::vector<std::string> &words);

		SDR learn(const std::vector<std::string> &words, float weightLearnRate, float inhibitionLearnRate, float inhibitionBiasLearnRate, float sparsity, int numLearnSteps);

		std::string getStringFromSDR(const SDR &sdr, float threshold);
	};
}