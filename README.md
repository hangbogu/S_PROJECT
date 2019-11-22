# S_PROJECT

## For Linux

### Prerequisites
CMake 3.11 or higher  
GCC 5.0 or higher  
Git   
libcurl configured to support HTTP/2 (libcurl does not support HTTP/2 by default. follow these instructions https://serversforhackers.com/c/curl-with-http2-support)  
AWS CLI configured correctly  

The below can be in different directories. 

#### PortAudio

Download the latest stable release of PortAudio from the website.  
Unzip.  
Follow the following terminal commands.  

$ cd portaudio  
$ mkdir build  
$ cd build  
$ cmake .. -DCMAKE_BUILD_TYPE=Release  
$ cmake --build .  
$ cmake --build . --target install  

#### AWS Transcribe

$ git clone https://github.com/aws/aws-sdk-cpp.git  
$ cd aws-sdk-cpp  
$ mkdir build  
$ cd build  
$ cmake .. -DBUILD_ONLY="transcribestreaming" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF  
$ cmake --build . --config Release  
$ sudo cmake --build . --config Release --target install  

#### Compiling

$ mkdir build  
$ cd build  
$ cmake .. -DCMAKE_BUILD_TYPE=Release  
$ cmake --build . --config Release  
