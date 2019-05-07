#ifndef _RECORDER_H_
#define _RECORDER_H_

#include "portaudio/include/portaudio.h"

#pragma comment(lib, "portaudio/lib/portaudio.lib")

#include <thread>
#include <array>
#include <fstream>

class Recorder
{
public:

	Recorder();
	~Recorder();

	bool init();
	void start();
	void pause();
	void stop() noexcept;

	bool isActive();

private:

	std::array<char, 1024 * sizeof(int16_t)> buffer;

	PaStream *stream = nullptr;
	PaError errorStatus{ 0 };
	PaStreamParameters inParams{};

	const double sampleRate{ 16000 };
	const unsigned long framesPerBuffer{ 1024 };

	std::ofstream outputFile;
	std::thread recordThread;
};

#endif