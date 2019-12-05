import wx
import _thread
import vlc
import os

from gtts import gTTS
from audio2 import Audio
from player import Player

class Example(wx.Frame):

    #initialize
    def __init__(self, *args, **kw):
        super(Example, self).__init__(*args, **kw)
        self.a = Audio()
        self.p = Player()
        self.InitUI()

        self.show_stt   = False # show speech to text
        self.wav_made   = False # True if audio file made
        self.mp3_made   = False
        self.filter     = False # setting to filter out profanity

    #initialize UI
    def InitUI(self):
        pnl = wx.Panel(self)
        self.button = wx.ToggleButton(pnl, label='Record',           pos=(20, 20))
        self.text   = wx.CheckBox    (pnl, label='Speech to Text',   pos=(20, 70))
        self.censor = wx.CheckBox    (pnl, label='Filter Profanity', pos=(20, 100))
        self.playt  = wx.Button      (pnl, label= 'Play',            pos= (107, 20))

        #Volume
        self.slider = wx.Slider    (pnl, 5, 50, 1, 100, (130, 130), (180, -1), style=wx.SL_LABELS)
        self.textv  = wx.StaticText(pnl, label= "Volume", pos= (45, 140))

	#Transcribe
        self.transcript = wx.TextCtrl(pnl, value= "Speech to text here... ", pos= (330, 45),
                          size=(300,150), style=wx.TE_MULTILINE | wx.TE_READONLY)
        self.textt      = wx.StaticText(pnl, label= "Transcription", pos= (330, 20))

        #Add Banned Words
        self.ban = wx.TextCtrl(pnl, value= "", pos= (330, 250), size=(300, 100), style= wx.TE_MULTILINE | wx.TE_PROCESS_ENTER)
        self.textb = wx.StaticText(pnl, label= "Add Banned Words", pos= (330, 225))

        #Bindings
        pnl.Bind        (wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        self.button.Bind(wx.EVT_TOGGLEBUTTON, self.OnButtonClicked)
        self.text.Bind  (wx.EVT_CHECKBOX,     self.OnSttClicked)
        self.slider.Bind(wx.EVT_SLIDER,       self.OnSliderMoved)
        self.playt.Bind (wx.EVT_BUTTON,       self.OnPlayClicked)
        self.censor.Bind(wx.EVT_CHECKBOX,     self.OnFilterClicked)
        self.ban.Bind   (wx.EVT_TEXT_ENTER,   self.OnBanEntered)
        self.sb = self.CreateStatusBar()

        #Window settings
        self.SetSize((650, 400))
        self.SetTitle('Profanity-Filtering Microphone')
        self.Centre()
        self.Show(True)

    #default
    def OnWidgetEnter(self, e):
        #default action
        name = e.GetEventObject().GetClassName()
        self.sb.SetStatusText(name + ' widget')
        e.Skip()

    #Filter Profanity Checkbox
    def OnFilterClicked(self, e):
        self.filter = e.GetEventObject().GetValue()
    
    #Speech to text checkbox
    def OnSttClicked(self, e):
        self.show_stt = e.GetEventObject().GetValue()

    #Play Button, plays audio and makes RAW file
    def OnPlayClicked(self, e):
        if self.filter == True:
            #censored
            myStt = gTTS(text=self.a.getCensored(), lang='en', slow='False')
            myStt.save('beeps.mp3')
            self.p.addAudio('beeps.mp3')
            self.mp3_made = True
        else:
            with open('input.wav', 'wb') as f:
                f.write(self.a.getWAV())
            self.p.addAudio('input.wav') 
            self.wav_made = True
        
        with open('input.raw', 'wb') as f:    #makes RAW after play
            f.write(self.a.getRAW())
        self.p.play()

    #Volume Slider
    def OnSliderMoved(self, e):
        self.p.setVol(self.slider.GetValue())

    #Record Button
    def OnButtonClicked(self, e):
        state = e.GetEventObject().GetValue()
        if state == True:
            e.GetEventObject().SetLabel("Stop")
            self.transcript.Clear()
            _thread.start_new_thread(self.record, ())
        else:
            e.GetEventObject().SetLabel("Record")
            if self.show_stt == True:
                if self.filter == True: 
                    self.transcript.SetValue(self.a.getCensored())
                    print("What you said filtered: " + self.a.getCensored() + "\n")
                else: self.transcript.SetValue(self.a.getText())
            else:
                self.transcript.SetValue("Speech to text here... ")

    def record(self):
        self.a.show_available_mic()
        self.a.record_mic_in(self.filter)

    #adds banned words to the database
    def OnBanEntered(self, e):
        myBanList = ""
        oneWord = False
        contents = self.ban.GetValue()
        comma = contents.find(",")
        space = contents.find(" ")
        if comma > -1:
            myBanList = contents.split(",")
        elif space > -1:
            myBanList = contents.split(" ")
        elif len(contents) > 0:
            myBanList = contents
            oneWord = True
        with open("datbase.txt", "a") as f:
            if f.mode == 'a':
                if oneWord == True:
                    f.write(myBanList + "\n")
                else:
                    for x in myBanList:
                        f.write(x + "\n")
            f.close()
        print("Banned words entered!")
        self.ban.Clear()

def main():
    app = wx.App()
    ex = Example(None)
    ex.Show()
    app.MainLoop()
    if ex.wav_made == True: os.remove('input.wav') #delete audio file after closing
    if ex.mp3_made == True: os.remove('beeps.mp3')

if __name__ == '__main__':
    main()
