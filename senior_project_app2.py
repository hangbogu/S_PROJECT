#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
ZetCode wxPython tutorial

This program creates a skeleton
of a file manager UI.

author: Jan Bodnar
website: zetcode.com
last edited: May 2018
"""

import wx
import _thread
import vlc
import os

"""
###CLASS Audio
###Purpose of this class is to have the app be ready to use a microphone...
###STATUS: FUNCTIONING
###FOLLOWED from EXAMPLE of website
###SRC: 
"""
from audio import Audio

class Example(wx.Frame):

    def __init__(self, *args, **kw):
        super(Example, self).__init__(*args, **kw)
        self.a = Audio()
        self.InitUI()

        self.show_stt   = False # show speech to text
        self.input_made = False # True if wav file made
        self.filter_pro = False # setting to filter out profanity

    def InitUI(self):
        pnl = wx.Panel(self)
        self.button = wx.ToggleButton(pnl, label='Record', pos=(20, 20))
        self.text = wx.CheckBox(pnl, label='Speech to Text', pos=(20, 70))
        #TODO: make censor work
        #      input audio from mic, save as audio file, transcribe
        #      if filter checked, send modifiied audio file, and modified transcribe
        self.censor = wx.CheckBox(pnl, label='Filter Profanity', pos=(20, 100))
        self.playt = wx.Button(pnl, label= 'Play', pos= (107, 20))

        #Volume
        self.slider = wx.Slider(pnl, 5, 50, 1, 100, (130, 130), (180, -1), style=wx.SL_LABELS)
        self.textv = wx.StaticText(pnl, label= "Volume", pos= (45, 140))
        self.Instance = vlc.Instance()
        self.player = self.Instance.media_player_new()

	#Record and transcribe
        self.transcript = wx.TextCtrl(pnl, value= "Speech to text here... ", pos= (330, 45),
                          size=(300,150), style=wx.TE_MULTILINE | wx.TE_READONLY)
        self.textt = wx.StaticText(pnl, label= "Transcription", pos= (330, 20))

        #Bindings
        pnl.Bind(wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        self.button.Bind(wx.EVT_TOGGLEBUTTON, self.OnButtonClicked)
        self.text.Bind(wx.EVT_CHECKBOX, self.OnSttClicked)
        self.slider.Bind(wx.EVT_SLIDER, self.OnSliderMoved)
        self.playt.Bind(wx.EVT_BUTTON, self.OnPlayClicked)
        self.censor.Bind(wx.EVT_CHECKBOX, self.OnFilterClicked)
        self.sb = self.CreateStatusBar()

        #Window settings
        self.SetSize((650, 300))
        self.SetTitle('Profanity-Filtering Microphone')
        self.Centre()
        self.Show(True)

    def OnWidgetEnter(self, e):
        #default action
        name = e.GetEventObject().GetClassName()
        self.sb.SetStatusText(name + ' widget')
        e.Skip()

    #Filter Profanity Checkbox
    def OnFilterClicked(self, e):
        self.filter_pro = e.GetEventObject().GetValue()
    
    #Speech to text checkbox
    def OnSttClicked(self, e):
        self.show_stt = e.GetEventObject().GetValue()

    #Play Button
    def OnPlayClicked(self, e):
        with open('input.wav', 'wb') as f:
            f.write(self.a.getAudio().get_wav_data())
        with open('input.raw', 'wb') as f:
            f.write(self.a.getRAW())
        self.player.set_mrl('input.wav') #makes RAW after play
        self.player.play()
        self.input_made = True

    #Volume Slider
    def OnSliderMoved(self, e):
        self.volume = self.slider.GetValue()
        self.player.audio_set_volume(self.volume)
        print("Volume: ", self.volume)

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
                self.transcript.SetValue(self.a.getText())
            else:
                self.transcript.SetValue("Speech to text here... ")

    def record(self):
        ##TODO:
        self.a.show_available_mic()
        self.a.record_mic_in(self.filter_pro)

def main():
    app = wx.App()
    ex = Example(None)
    ex.Show()
    app.MainLoop()
    if ex.input_made == True:
        os.remove('input.wav') #delete .wav file after closing

if __name__ == '__main__':
    main()
