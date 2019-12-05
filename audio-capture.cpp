#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/model/AudioStream.h>
#include <csignal>
#include <cstdio>
#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <string>

using SampleType = int16_t;
extern int SampleRate;
int Finished = paContinue;
Aws::Utils::Threading::Semaphore pasignal(0 /*initialCount*/, 1 /*maxCount*/);

static int AudioCaptureCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    auto stream = static_cast<Aws::TranscribeStreamingService::Model::AudioStream*>(userData);
    const auto beg = static_cast<const unsigned char*>(inputBuffer);
    const auto end = beg + framesPerBuffer * sizeof(SampleType);

    (void)outputBuffer; // Prevent unused variable warnings
    (void)timeInfo;
    (void)statusFlags;
 
    Aws::Vector<unsigned char> bits { beg, end };
    Aws::TranscribeStreamingService::Model::AudioEvent event(std::move(bits));
    stream->WriteAudioEvent(event);
    auto rawOutput = event.GetAudioChunk();
    std::string pathname("a.raw");
    std::ofstream textout(pathname.c_str(), std::ios::out | std::ios::app | std::ios::binary);
    textout.write((const char*)&rawOutput[0], rawOutput.size()); 
    textout.close();

    if (Finished == paComplete) {
        pasignal.Release(); // signal the main thread to close the stream and exit
    }

    return Finished;
}

void interruptHandler(int)
{
    Finished = paComplete;
}
 
int CaptureAudio(Aws::TranscribeStreamingService::Model::AudioStream& targetStream)
{
 
    signal(SIGINT, interruptHandler); // handle ctrl-c
    PaStreamParameters inputParameters;
    PaStream* stream;
    PaError err = paNoError;
 
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "Error: Failed to initialize PortAudio.\n");
        return -1;
    }

    inputParameters.device = Pa_GetDefaultInputDevice(); // default input device
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        Pa_Terminate();
        return -1;
    }

    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // start the audio capture
    err = Pa_OpenStream(&stream, &inputParameters, nullptr, /* &outputParameters, */
        SampleRate, paFramesPerBufferUnspecified,
        paClipOff, // you don't output out-of-range samples so don't bother clipping them.
        AudioCaptureCallback, &targetStream);

    if (err != paNoError) {
        fprintf(stderr, "Failed to open stream.\n");        
        goto done;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Failed to start stream.\n");
        goto done;
    }
    printf("=== Now recording!! Speak into the microphone. ===\n");
    fflush(stdout);

    if ((err = Pa_IsStreamActive(stream)) == 1) {
        pasignal.WaitOne();
    }
    if (err < 0) {
        goto done;
    }

    Pa_CloseStream(stream);

done:
    Pa_Terminate();
    return 0;
}
