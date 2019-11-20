#include <aws/core/Aws.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/TranscribeStreamingServiceClient.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionHandler.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>
#include <cstdio>
#include <map>
#include <string>

using namespace Aws;
using namespace Aws::TranscribeStreamingService;
using namespace Aws::TranscribeStreamingService::Model;

int SampleRate = 16000; // 16 Khz
int CaptureAudio(AudioStream& targetStream);

int main()
{
    std::map<std::string, bool> wordMap;
    wordMap.insert(std::pair<std::string, bool>("Hello", true)); 
    wordMap.insert(std::pair<std::string, bool>("hello", true)); 
    wordMap.insert(std::pair<std::string, bool>("Hello.", true)); 
    wordMap.insert(std::pair<std::string, bool>("hello.", true)); 
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
 		    printf("time: %lf", r.GetEndTime());
                    printf("[partial] ");
                } else {
                    printf("[Final] ");
                }
                for (auto&& alt : r.GetAlternatives()) {
                    for (auto&& items : alt.GetItems()){
                      if(wordMap.find(items.GetContent()) == wordMap.end()) printf("%s\n", items.GetContent().c_str());
                    } 
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
