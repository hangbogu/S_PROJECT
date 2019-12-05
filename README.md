# S_PROJECT (CMPE 195F Group 1 Project U19: Profanity Filtering Microphone)

## Getting Started
This project is written in Python 3 and works for Ubuntu 18.04 and Windows OS. To get this project working on your computer, download all files in this project. Then download the prerequisites and follow the installation instructions below.

Note that this repo contains both the front-end and back-end, which should be placed in different locations. The front-end should be installed locally and the back-end should be installed in an AWS instance 

### Prerequisites
For Ubuntu 18.04:
Make sure you are a non-root user that has sudo privileges. Ubuntu comes with Python 3, so run the following code to make sure the version of Python is up-to-date.
```
$ sudo apt update
$ sudo apt -y upgrade 
```
Then run the following code to check the version of Python 3 that is installed on your system. The version should be Python 3.7 or above.
```
$ python3 -V
```
Then download the following libraries. 
For the GUI, install wxPython using the following code.
```
$ sudo pip install -U wxPython
```
To access the microphone for audio input and functions related to audio, install the PyAudio library.
```
$ sudo pip install PyAudio
```
For the speech recognition used by the front end, install Python's SpeechRecognition library. This will be used to record and process the audio. This will also create the RAW file that will be sent to the backend.
```
$ sudo pip install SpeechRecognition
```
Lastly, for the media player install the Python VLC video player.
```
$ sudo pip install python-vlc
```
After adding the front-end components locally, add the following code to connect to the backend.

#### LibCurl with nghttp2

Run the following commands to access the AWS S3 Instance and Transcribe API.
```
$ sudo apt-get -y install build-essential nghttp2 libnghttp2-dev libssl-dev   
$ wget https://curl.haxx.se/download/curl-7.63.0.tar.gz   
$ tar xzf curl-7.63.0.tar.gz    
$ cd curl-7.63.0    
$ ./configure --with-nghttp2 --prefix=/usr/local --with-ssl   
$ make && sudo make install   
$ sudo ldconfig   
```

#### PortAudio

Download the latest stable release of PortAudio from the website.  
Unzip.  
Input the following terminal commands.  

```
$ cd portaudio  
$ mkdir build  
$ cd build  
$ cmake .. -DCMAKE_BUILD_TYPE=Release  
$ cmake --build .  
$ cmake --build . --target install  
```

#### AWS Transcribe

Input the following terminal commands.
```
$ git clone https://github.com/aws/aws-sdk-cpp.git  
$ cd aws-sdk-cpp  
$ mkdir build  
$ cd build  
$ cmake .. -DBUILD_ONLY="transcribestreaming" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF  
$ cmake --build . --config Release  
$ sudo cmake --build . --config Release --target install  
```

#### CMake compile (Back-end)

Input the following terminal commands after doing all of the above.
$ mkdir build  
$ cd build  
$ cmake .. -DCMAKE_BUILD_TYPE=Release  
$ cmake --build . --config Release  

The instructions to setup the AWS Instance are far too long for this README, and thus will be summarized as follows.

#### Amazon Web Services
Follow the instructions found here. (aws.amazon.com/ec2/getting-started)
Make sure to choose a Linux server container with size "macro" or larger.

Once the instance is set up, follow the instructions below for testing.
(docs.aws.amazon.com/transcribe/latest/dg/getting-started.html)

#### Pytorch-Kaldi
The following will be done in the AWS Instance.

First, install Kaldi with the directions found here. [Kaldi](https://github.com/kaldi-asr/kaldi)
Next, install PyTorch with the directions found here. [PyTorch](https://pytorch.org)
Then, install PyTorch-Kaldi with the directions found here. [PyTorch-Kaldi](https://github.com/mravanelli/pytorch-kaldi)

After this, the instance should be set with the default training model.

Last, we will need training data. Any data set with around 1000 hours of English data is sufficient. The one used for this project can be found here. [LibriSpeech](www.openslr.org/12)

### Installing
To run the project, navigate to the directory the project folder is in.
Example:
```
$ cd (your path here)/S_PROJECT
```
Then run the following code to open the UI.
```
python3 senior_proj_app.py
```
Press the Record button. In the terminal it will say the name of your system's microphone. Copy the first name that shows up and paste it in place of the microphone name found in audio.py. This will be in the initializer.
Example:
```
def __init__(self, mic_name="Intel 82801AA-ICH: - (hw:0,0)"):
```

## Running Tests
Open the UI again. Select your settings with the check boxes. Speech to Text will show the transcription of what you said in the transcription box. Filter Profanity will show if filtering profanity was selected or not in the terminal. The volume adjusts the playback volume of the media player when you press the Play button. Record will listen to the microphone for speech input. The terminal will show "Say something" and return with what you said or that it couldn't understand the audio. After the terminal returns either of these options, you can press the Stop button. Depending on your settings, the output will show in the UI. Play will playback what you said, and create a RAW file to send to the backend.

Back end tests can be found in SENIOR_PROJECT folder and pytorch_kaldi once binaries are recompiled in AWS instance.

## Built With
* [wxPython](https://wxpython.org/pages/downloads/index.html) - For GUI
* [PyAudio](https://pypi.org/project/PyAudio/) - For microphone input and functions
* [SpeechRecognition](https://pypi.org/project/SpeechRecognition/) - For the front end's speech recognition
* [python-vlc](https://pypi.org/project/python-vlc/) - For the media player

* [PortAudio](https://www.portaudio.com) - Audio API 
* [Libcurl](https://curl.haxx.se/libcurl) - URL transfer library for AWS Instance connection
* [nghttp2](https://nghhttp2.org) - Implementation of HTTP/2 for libcurl efficiency and multiplexing requests over TCP from AWS instance to local project.

* [Kaldi](https://kaldi-asr.org) - Speech recognition toolkit to handle FST
* [PyTorch](https://pytorch.org) - Open source machine learning framework
* [PyTorch-Kaldi](https://github.com/mravanelli/pytorch-kaldi) - DNN/RNN hybrid speech recognition system.

## Authors
* **Arturo Reyes** - *Front-end and UI/UX, audio.py, and senior_proj_app.py* 
* **Michelle Roque** - *Front-end functionality, senior_proj_app2.py*
* **Hangbo Gu** - *Back-end, audio-capture.cpp and main.cpp*
* **David Tran** - *Back-end, AWS Instance, pytorch-kaldi (custom)*

## Acknowlegements
* wxPython Tutorial from (http://zetcode.com/wxpython/)
* SpeechRecognition Tutorial for audio module from (https://pypi.org/project/SpeechRecognition/)
* Dan Povey from (https://danielpovey.com)
* README template from [here](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#project-title)
@inproceedings{pytorch-kaldi,
title    = {The PyTorch-Kaldi Speech Recognition Toolkit},
author    = {M. Ravanelli and T. Parcollet and Y. Bengio},
booktitle    = {In Proc. of ICASSP},
year    = {2019}
}
