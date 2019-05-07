#include <cstdio>
#include <array>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

#include "Recorder.h"

namespace
{
	int inputCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData)
	{
		std::ofstream *pFile = static_cast<std::ofstream*>(userData);
		pFile->write((char*)inputBuffer, framesPerBuffer * sizeof(int16_t));
		return 0;
	}
}

Recorder::Recorder()
{
	if (!init())
	{
		fprintf(stderr, "Recorder init failed: 0x%02x\n", errorStatus);
	}
}

Recorder::~Recorder()
{
	stop();
	errorStatus = Pa_Terminate();
}

bool Recorder::init()
{
	errorStatus = Pa_Initialize();

	if (errorStatus)
	{
		fprintf(stderr, "Pa_Initialize failed: 0x%02x\n", errorStatus);
		return false;
	}

	int deviceCount = Pa_GetDeviceCount();
	const PaDeviceInfo *deviceInfo = nullptr;

	for (int i = 0; i < deviceCount; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("[%d] [%s]\n", i, deviceInfo->name);
	}

	inParams.device = Pa_GetDefaultInputDevice();
	inParams.channelCount = 1;
	inParams.sampleFormat = paInt16;
	inParams.suggestedLatency = deviceInfo->defaultLowInputLatency;
	inParams.hostApiSpecificStreamInfo = nullptr;

	outputFile.open("output.raw", std::ios::binary);

	errorStatus = Pa_OpenStream(
		&stream,
		&inParams,
		/*&outparams*/ nullptr,
		sampleRate,
		framesPerBuffer,
		paClipOff,
		inputCallback,
		&outputFile);

	printf("Recording started...\n");

	return true;
}

void Recorder::start()
{
	errorStatus = Pa_StartStream(stream);
	if (errorStatus)
	{
		fprintf(stderr, "Recorder start failed: 0x%02x\n", errorStatus);
		return;
	}
}

void Recorder::pause()
{
	Pa_StopStream(stream);
}

void Recorder::stop() noexcept
{
	recordThread.~thread();
}

bool Recorder::isActive()
{
	return Pa_IsStreamActive(stream);
}