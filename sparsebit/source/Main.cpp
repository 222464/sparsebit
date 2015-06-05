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

int main()
{
	// config
	int inputHeight = 10;
	int inputWidth = 10;
	int width = 20;
	int height = 20;
	float connectionRadius = 6.0f;
	float inhibitionRadius = 6.0f;

	std::mt19937 generator;
	generator.seed(time(nullptr));

	nlp::WTSDR wtsdr;
	wtsdr.create(inputWidth, inputHeight, width, height, connectionRadius, inhibitionRadius, generator);

	std::string input = "Machine learning is a scientific discipline that explores the construction and study of algorithms that can learn from data";
	std::string input2 = "Machine learning can help solve practical problems found in everyday life";

	std::vector<std::string> words;
	std::vector<std::string> words2;

	std::string sub = "";

	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == ' ' || i == input.length() - 1)
		{
			if (i == input.length() - 1)
				sub += input[i];

			words.push_back(sub);
			sub = "";
			continue;
		}

		sub += input[i];
	}
	sub = "";

	for (int i = 0; i < input2.length(); i++)
	{
		if (input2[i] == ' ' || i == input2.length() - 1)
		{
			if (i == input2.length() - 1)
				sub += input2[i];

			words2.push_back(sub);
			sub = "";
			continue;
		}

		sub += input2[i];
	}

	SDR sdr = wtsdr.learn(words, 0.05f, 0.1f, 0.001f, 0.2f, 300);

	SDR sdr2 = wtsdr.learn(words2, 0.05f, 0.1f, 0.001f, 0.2f, 300);

	vis::PrettySDR sdrRenderer;
	sdrRenderer.create(width, height);

	vis::PrettySDR sdrRenderer2;
	sdrRenderer2.create(width, height);

	vis::PrettySDR sdrRenderer3;
	sdrRenderer3.create(width, height);

	for (int i = 0; i < sdr._height; i++)
	{
		for (int j = 0; j < sdr._width; j++)
		{
			sdrRenderer[j + i * sdr._width] = sdr._states[j + i * sdr._width];
		}
	}

	for (int i = 0; i < sdr2._height; i++)
	{
		for (int j = 0; j < sdr2._width; j++)
		{
			sdrRenderer2[j + i * sdr2._width] = sdr2._states[j + i * sdr2._width];
		}
	}

	SDR overlap;
	overlap._height = sdr._height;
	overlap._width = sdr._width;

	for (int i = 0; i < sdr._height; i++)
	{
		for (int j = 0; j < sdr._width; j++)
		{
			overlap._states.push_back(sdr._states[j + i * sdr._width] * sdr2._states[j + i * sdr._width]);
			sdrRenderer3[j + i * sdr._width] = sdr._states[j + i * sdr._width] * sdr2._states[j + i * sdr._width];
		}
	}

	std::string overlapStr = wtsdr.getStringFromSDR(overlap, 0.99f);

	std::cout << "Overlap String: " << overlapStr << std::endl;

	sf::RenderWindow window(sf::VideoMode(1600, 1000), "SDR Testing");

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
		sdrRenderer2.draw(window, sf::Vector2f(410.0f, 10.0f));
		sdrRenderer3.draw(window, sf::Vector2f(225.0f, 410.0f));
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

			timePassed += deltaTime;

			ticks++;
		}
	}

	return 0;
}