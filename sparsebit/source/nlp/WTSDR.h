#pragma once

#include "SDRT.h"

namespace nlp
{
	class WTSDR
	{
	private:
		SDRT _sdrt;

	public:
		void create(int inputWidth, int inputHeight, int width, int height, float connectionRadius, float inhibitionRadius, float inhibitionThreshold, std::mt19937 &generator);

		SDR convertStringToSDR(const std::string &word, const std::string &definition);
	};
}