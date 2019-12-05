#include <aws/core/Aws.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/TranscribeStreamingServiceClient.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionHandler.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>
#include <cstdio>
#include <locale>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "pa_util.h"

#define PA_SAMPLE_TYPE  paInt16

static PaTime stream_begin;
static PaTime output_time;
static bool first = true;
static double count = 0;
static std::vector<std::pair<double, double>> time_vector;
static ring_buffer_size_t rbs_min(ring_buffer_size_t a, ring_buffer_size_t b)
{
    return (a < b) ? a : b;
}


typedef struct
{
    unsigned            frameIndex;
    int                 threadSyncFlag;
    short             *ringBufferData;
    PaUtilRingBuffer    ringBuffer;
    FILE               *file;
    pthread_t          threadHandle;
}
paTestData;

static int threadFunctionReadFromRawFile(void* ptr)
{
    paTestData* pData = (paTestData*)ptr;
    while (1)
    {
        ring_buffer_size_t elementsInBuffer = PaUtil_GetRingBufferWriteAvailable(&pData->ringBuffer);

        if (elementsInBuffer >= pData->ringBuffer.bufferSize / 4)
        {
            void* ptr[2] = {0};
            ring_buffer_size_t sizes[2] = {0};

            PaUtil_GetRingBufferWriteRegions(&pData->ringBuffer, elementsInBuffer, ptr + 0, sizes + 0, ptr + 1, sizes + 1);

            if (!feof(pData->file))
            {
                ring_buffer_size_t itemsReadFromFile = 0;
                int i;
                for (i = 0; i < 2 && ptr[i] != NULL; ++i)
                {
                    itemsReadFromFile += (ring_buffer_size_t)fread(ptr[i], pData->ringBuffer.elementSizeBytes, sizes[i], pData->file);
                }
                PaUtil_AdvanceRingBufferWriteIndex(&pData->ringBuffer, itemsReadFromFile);

                /* Mark thread started here, that way we "prime" the ring buffer before playback */
                pData->threadSyncFlag = 0;
            }
            else
            {
                /* No more data to read */
                pData->threadSyncFlag = 1;
                break;
            }
        }

        /* Sleep a little while... */
        Pa_Sleep(20);
    }

    return 0;
}

static PaError startThread( paTestData* pData, void* fn )
{
    pthread_attr_t tattr;
    struct sched_param param;
    param.sched_priority = 0;
    int ret = pthread_attr_init (&tattr);
    ret = pthread_attr_setschedparam (&tattr, &param);
    pData->threadSyncFlag = 1;
    pData->threadHandle = pthread_create(&(pData->threadHandle), &tattr, (void*(*)(void*))fn, (void*)pData);
    while (pData->threadSyncFlag) {
        Pa_Sleep(10);
    }
    return paNoError;
}

static int stopThread( paTestData* pData )
{
    pData->threadSyncFlag = 1;
    while (pData->threadSyncFlag) {
        Pa_Sleep(10);
    }
    pthread_cancel(pData->threadHandle);
    pData->threadHandle = 0;
    return paNoError;
}


/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{

    paTestData *data = (paTestData*)userData;
    ring_buffer_size_t elementsToPlay = PaUtil_GetRingBufferReadAvailable(&data->ringBuffer);
    ring_buffer_size_t elementsToRead = rbs_min(elementsToPlay, (ring_buffer_size_t)(framesPerBuffer));
    short* wptr = (short*)outputBuffer;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) userData;
    //if(timeInfo->currentTime > 1 && timeInfo->currentTime < 5) printf("hi");
    //if(count < 5) for(int i = 0; i < elementsToRead; i++){
    //  *(data->ringBufferData) = 0;
    //  data->ringBufferData++;
    //  count++;
    //}
    //data = (paTestData*)userData;
    if(first) {
      stream_begin = timeInfo->outputBufferDacTime;
      output_time = stream_begin;
      first = false;
    }
    else output_time = timeInfo->outputBufferDacTime; 
    bool mute = false;
    for(int i = 0; i < time_vector.size(); i++){
      if(time_vector[i].first < output_time - stream_begin && time_vector[i].second > output_time - stream_begin){
        mute = true;
      }
    }
    if(mute){ 
      for(int i = 0; i < framesPerBuffer; i++) *wptr++ = 0;
      data->frameIndex += framesPerBuffer;
      PaUtil_FlushRingBuffer(&data->ringBuffer);
      printf("mute %lf\n", output_time - stream_begin);
    }
    else data->frameIndex += PaUtil_ReadRingBuffer(&data->ringBuffer, wptr, elementsToRead);

    return data->threadSyncFlag ? paComplete : paContinue;
}

static unsigned NextPowerOf2(unsigned val)
{
    val--;
    val = (val >> 1) | val;
    val = (val >> 2) | val;
    val = (val >> 4) | val;
    val = (val >> 8) | val;
    val = (val >> 16) | val;
    return ++val;
}


using namespace Aws;
using namespace Aws::TranscribeStreamingService;
using namespace Aws::TranscribeStreamingService::Model;

int SampleRate = 16000; // 16 Khz
int CaptureAudio(AudioStream& targetStream);

int main()
{
    std::map<std::string, bool> wordMap;
    std::string line;   
    std::ifstream myfile("list.txt");
    if (myfile.is_open()){
      while(getline(myfile, line)){
        wordMap.insert(std::pair<std::string, bool>(line, true)); 
      }
    }
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration config;
#ifdef _WIN32
        config.httpLibOverride = Aws::Http::TransferLibType::WIN_INET_CLIENT;
#endif
        TranscribeStreamingServiceClient client(config);
        StartStreamTranscriptionHandler handler;
        handler.SetTranscriptEventCallback([wordMap](const TranscriptEvent& ev) {
            for (auto&& r : ev.GetTranscript().GetResults()) {
                if (r.GetIsPartial()) {
 		    printf("time: %lf\n", r.GetEndTime());
                } else {
                    printf("Final Transcription");
                    PaStreamParameters  inputParameters,
                        outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data = {0};
    unsigned            delayCntr;
    unsigned            numSamples;
    unsigned            numBytes;

    /* We set the ring buffer size to about 500 ms */
    numSamples = NextPowerOf2((unsigned)(16000 * 0.1));
    numBytes = numSamples * sizeof(short);
    data.ringBufferData = (short *) PaUtil_AllocateMemory( numBytes );
    if( data.ringBufferData == NULL )
    {
        printf("Could not allocate ring buffer data.\n");
        goto done;
    }

    if (PaUtil_InitializeRingBuffer(&data.ringBuffer, sizeof(short), numSamples, data.ringBufferData) < 0)
    {
        printf("Failed to initialize ring buffer. Size is not power of 2 ??\n");
        goto done;
    }

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    /* Playback recorded data.  -------------------------------------------- */
    data.frameIndex = 0;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto done;
    }
    outputParameters.channelCount = 1;                     
    outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    printf("\n=== Now playing back from file a.raw until end-of-file is reached ===\n"); fflush(stdout);
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              16000,
              2000,
              paClipOff,      
              playCallback,
              &data );
    if( err != paNoError ) goto done;

    if( stream )
    {
        /* Open file again for reading */
        data.file = fopen("a.raw", "rb");
        if (data.file != 0)
        {
            /* Start the file reading thread */
            err = startThread(&data, (void*)threadFunctionReadFromRawFile);
            if( err != paNoError ) goto done;

            err = Pa_StartStream( stream );
            if( err != paNoError ) goto done;

            printf("Waiting for playback to finish.\n"); fflush(stdout);

            /* The playback will end when EOF is reached */
            while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) {
                printf("index = %d\n", data.frameIndex); 
                fflush(stdout);
                Pa_Sleep(1000);
            }
            if( err < 0 ) goto done;
        }
        
        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto done;

        fclose(data.file);
        
        printf("Done.\n"); fflush(stdout);
    }

done:
    Pa_Terminate();
    std::remove("a.raw");
    if( data.ringBufferData )
        PaUtil_FreeMemory( data.ringBufferData );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
                }
                for (auto&& alt : r.GetAlternatives()) {
                    for (auto&& items : alt.GetItems()){
                      std::locale loc;
                      std::string a = items.GetContent();
                      for(int i = 0; i < a.length(); i++) a[i] = std::tolower(a[i], loc); 
                      if(wordMap.find(items.GetContent()) == wordMap.end()) {
                        printf("%s ", items.GetContent().c_str());
                      }
                      else{
                        time_vector.push_back(std::make_pair(items.GetStartTime(), items.GetEndTime()));
                        printf("%lf, %lf\n", time_vector.back().first, time_vector.back().second);
		      }
                    } 
                    printf("\n");
                    //printf("%s\n", alt.GetTranscript().c_str());
                }
            }
        });

        StartStreamTranscriptionRequest request;
        request.SetMediaSampleRateHertz(SampleRate);
        request.SetLanguageCode(LanguageCode::en_US);
        request.SetMediaEncoding(MediaEncoding::pcm);
        request.SetEventStreamHandler(handler);

        auto OnStreamReady = [](AudioStream& stream) {
            CaptureAudio(stream);
            stream.flush();
            stream.Close();
        };

        Aws::Utils::Threading::Semaphore signaling(0 /*initialCount*/, 1 /*maxCount*/);
        auto OnResponseCallback = [&signaling](const TranscribeStreamingServiceClient*,
                  const Model::StartStreamTranscriptionRequest&,
                  const Model::StartStreamTranscriptionOutcome&,
                  const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) { signaling.Release(); };

        client.StartStreamTranscriptionAsync(request, OnStreamReady, OnResponseCallback, nullptr /*context*/);
        signaling.WaitOne(); // prevent the application from exiting until we're done
    }

    Aws::ShutdownAPI(options);

    return 0;
}
