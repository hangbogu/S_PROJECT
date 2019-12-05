class Audio:
    import speech_recognition as sr

    mic_name = ""
    text     = ""
    beeps    = ""
    audio    = None
    r        = None

    #initialize recognizer and mic name
    def __init__(self, mic_name="Intel 82801AA-ICH: - (hw:0,0)"):
        self.r = self.sr.Recognizer()
        self.mic_name = mic_name

    #record mic in, find device id
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
            self.write_audio(censor)

    def write_audio(self, censorw=False):
        try:
            self.text = self.r.recognize_google(self.audio)
            print("you said: "+ self.text)
            if censorw == True:
                with open("datbase.txt", "r") as f:
                    if f.mode == 'r':
                        contents = f.read()
                        mylist = contents.split("\n")
                        mytext = self.text.split(" ")
                        self.beeps = ""
                        for x in mylist:
                            for n, i in enumerate(mytext):
                               if i == x: mytext[n] = "beep"
                        for z in mytext: self.beeps += z + " "
                    f.close()
        except self.sr.UnknownValueError:
            print("Google speech recognition could not understand audio")
            self.text = "Speech to text here..."
        except self.sr.RequestError as e:
            print("could not request results from Google Speech Recognition service; {0}".format(e))
            self.text = "Speech to text here..."

    def show_available_mic(self):
        for index, name in enumerate(self.sr.Microphone.list_microphone_names()):
            print("microphone with name \"{1}\" found for 'Microphne(device_index={0})'".format(index,name))

    def getAudio(self):
        return self.audio

    def getText(self):
        return self.text

    def getRAW(self):
        return self.audio.get_raw_data()

    def getWAV(self):
        return self.audio.get_wav_data()

    #returns censored version of text
    def getCensored(self):
        return self.beeps
        

