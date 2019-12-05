
class Audio:
    import speech_recognition as sr

    mic_name = ""
    text = ""
    audio = None
    r = None

    def __init__(self, mic_name="Intel 82801AA-ICH: - (hw:0,0)"):
        self.r = self.sr.Recognizer()
        self.mic_name = mic_name

    def record_mic_in(self, censor=False):
        device_id = 0
        for index, name in enumerate(self.sr.Microphone.list_microphone_names()):
            if self.mic_name == name:
                device_id = index
        with self.sr.Microphone(device_index=device_id, sample_rate=48000) as source:
            self.r.adjust_for_ambient_noise(source, duration=1)
            if censor == True:
                print("Filter Profanity on")
            else:
                print("Filter Profanity off")
            print("Say something")
            self.audio = self.r.listen(source)
            self.write_audio()

    def write_audio(self):
        try:
            text = self.r.recognize_google(self.audio)
            print("you said: "+text)
        except self.sr.UnknownValueError:
            print("Google speech recognition could not understand audio")
        except self.sr.RequestError as e:
            print("could not request results from Google Speech Recognition service; {0}".format(e))

    def show_available_mic(self):
        for index, name in enumerate(self.sr.Microphone.list_microphone_names()):
            print("microphone with name \"{1}\" found for 'Microphne(device_index={0})'".format(index,name))

    def getAudio(self):
        return self.audio

    def getText(self):
        return self.r.recognize_google(self.audio)

    def getRAW(self):
        return self.audio.get_raw_data()
        
