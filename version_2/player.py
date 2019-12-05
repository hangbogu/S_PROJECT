import vlc

class Player:
    player = None

    def __init__(self):
        instance = vlc.Instance()
        self.player = instance.media_player_new()

    def addAudio(self, file_name):
        self.player.set_mrl(file_name)

    def play(self):
        self.player.play()

    def setVol(self, volume):
        self.player.audio_set_volume(volume)
        print("Volume: ", volume)
