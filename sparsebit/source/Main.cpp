#include <windows.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <iomanip>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "nlp/WTSDR.h"
#include "vis/PrettySDR.h"

void print(std::wstring &str)
{
	DWORD numCharsToWrite = str.length();
	LPDWORD numCharsWritten = NULL;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), numCharsToWrite, numCharsWritten, NULL);
}

void printSDR(const SDR &sdr)
{
	std::wstring output = L"";
	std::wstring block = L"\u2588\u2588";
	std::wstring topBorder = L"--";
	std::wstring bottomBorder = L"--";
	std::wstring leftBorder = L"|";
	std::wstring rightBorder = L"|";

	output += leftBorder;
	for (int col = 0; col < std::round(sdr._width / 2.0f) - (std::round(std::to_wstring(sdr._width).length() / 2.0f) + std::round(std::to_wstring(sdr._height).length() / 2.0f)); col++)
		output += topBorder;
	output += L"[" + std::to_wstring(sdr._width) + L", " + std::to_wstring(sdr._height) + L"]";
	for (int col = std::round(sdr._width / 2.0f) + 2; col < sdr._width; col++)
		output += topBorder;
	output += rightBorder;
	output += L"\n";

	for (int row = 0; row < sdr._height; row++)
	{
		output += leftBorder;
		for (int col = 0; col < sdr._width; col++)
			if (sdr._states[col + row * sdr._width] > 0.5f)
				output += block;
			else
				output += L"  ";
		output += rightBorder;
		output += L"\n";
	}

	output += leftBorder;
	for (int col = 0; col < sdr._width; col++)
		output += bottomBorder;
	output += rightBorder;
	output += L"\n";

	print(output);
}

void learnString(SDRT &sdrt, SDR &sdr, int inputWidth, int inputHeight, const std::string &str) {
	std::vector<float> inputs(inputWidth * inputHeight);
	std::unordered_map<char, float> inputMap;

	std::cout << "Input string:" << std::endl;
	std::cout << "\"" << str << "\"" << std::endl;

	float inputMapCounter = 0.0f;

	for (int i = 0; i < str.length(); i++)
	{
		if (inputMap.find(str[i]) == inputMap.end())
		{
			inputMap.emplace(str[i], inputMapCounter);
			inputMapCounter++;
		}
	}

	for (int i = 0; i < str.size(); i++)
	{
		inputs[i] = inputMap.at(str[i]);
	}

	printSDR(sdr);

	std::cout << "Learning..." << std::endl;

	for (int i = 0; i < 100; i++)
	{
		sdrt.learn(inputs, sdr, 0.05f, 0.5f);
		sdr = sdrt.generateSDR(inputs);
	}

	std::cout << "Reconstruction:" << std::endl;
	std::vector<float> recon = sdrt.reconstruct(sdr);
	std::string reconStr = "";
	for (int i = 0; i < str.length(); i++)
	{
		reconStr += recon[i] > 0.5f ? " " : "#";
	}

	std::cout << reconStr << std::endl;
}

int main()
{
	// config
	int inputHeight = 4;
	int inputWidth = 4;
	int width = 10;
	int height = 10;
	float connectionRadius = 3.0f;
	float inhibitionRadius = 4.0f;
	float inhibitionThreshold = 4.0f;

	std::mt19937 generator;
	generator.seed(time(nullptr));

	nlp::WTSDR wtsdr;
	wtsdr.create(inputWidth, inputHeight, width, height, connectionRadius, inhibitionRadius, inhibitionThreshold, generator);

	SDR sdr;

	vis::PrettySDR sdrRenderer;
	sdrRenderer.create(width, height);

	sf::RenderWindow window(sf::VideoMode(800, 600), "SDR Testing");

	float timeStep = 1.0f / 60.0f * 1000.0f;
	float timePassed = 0.0f;
	float deltaTime = 0.0f;

	int maxTicks = 6;
	int ticks = 0;

	sf::Clock clock;
	clock.restart();

	while (window.isOpen())
	{
		window.clear();
		sdrRenderer.draw(window, sf::Vector2f(10.0f, 10.0f));
		window.display();

		timePassed = 0.0f;
		ticks = 0;

		deltaTime = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		while (timePassed < timeStep && ticks < maxTicks)
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				// "close requested" event: we close the window
				if (event.type == sf::Event::Closed)
					window.close();
			}

			// update here
			for (int i = 0; i < sdr._height; i++)
			{
				for (int j = 0; j < sdr._width; j++)
				{
					sdrRenderer[j + i * sdr._width] = sdr._states[j + i * sdr._width];
				}
			}

			timePassed += deltaTime;

			ticks++;
		}
	}

	return 0;
}