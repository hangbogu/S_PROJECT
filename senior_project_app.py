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


    def InitUI(self):
        pnl = wx.Panel(self)

        self.button = wx.Button(pnl, label='Reocrd', pos=(20, 20))
        text = wx.CheckBox(pnl, label='Speech to Text', pos=(20, 90))
        combo = wx.ComboBox(pnl, pos=(120, 22), choices=['Python', 'Ruby'])
        #TODO: fix slider
        slider = wx.Slider(pnl, 5, 6, 1, 10, (120, 90), (110, -1))

        pnl.Bind(wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        self.button.Bind(wx.EVT_BUTTON, self.onButtonClicked)
        text.Bind(wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        combo.Bind(wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        slider.Bind(wx.EVT_ENTER_WINDOW, self.OnWidgetEnter)
        self.sb = self.CreateStatusBar()

        self.SetSize((650, 400))
        self.SetTitle('wx.Statusbar')
        self.Centre()
        self.Show(True)

    def OnWidgetEnter(self, e):
        #default action
        name = e.GetEventObject().GetClassName()
        self.sb.SetStatusText(name + ' widget')
        e.Skip()

    def onSliderMoved(self, e):
        #TODO: make this function control volume...
        name = e.GetEventOject().GetClassName()
        self.sb.SetStatusText(name + ' widget')
        e.Skip()

    def record(self):
        #TODO:
        self.a.record_mic_in()
        self.button.Enable(True)

    def onButtonClicked(self, e):
        #TODO:
        self.button.Enable(False)
        _thread.start_new_thread(self.record, ())


def main():

    app = wx.App()
    ex = Example(None)
    ex.Show()
    app.MainLoop()


if __name__ == '__main__':
    main()
