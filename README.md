# S_PROJECT

## For Linux

### Prerequisites
<<<<<<< HEAD
CMake 3.11 or higher  
GCC 5.0 or higher  
Git   
libcurl configured to support HTTP/2 (libcurl does not support HTTP/2 by default. follow these instructions https://serversforhackers.com/c/curl-with-http2-support)  
AWS CLI configured correctly with an AWS account  

The below can be in different directories. 
=======
Git   
CMake 3.11 or higher  
GCC 5.0 or higher  

Note: Enter "software-properties-gtk" in Terminal and check "Source code" box, then run "sudo get-apt update".
This should solve any installation issues with a fresh Linux installation.

AWS CLI configured correctly with an AWS account  

The below can be located in different directories. 

#### LibCurl with nghttp2

$ sudo apt-get -y install build-essential nghttp2 libnghttp2-dev libssl-dev   
$ wget https://curl.haxx.se/download/curl-7.63.0.tar.gz   
$ tar xzf curl-7.63.0.tar.gz    
$ cd curl-7.63.0    
$ ./configure --with-nghttp2 --prefix=/usr/local --with-ssl   
$ make && sudo make install   
$ sudo ldconfig   
>>>>>>> 12472e5d60b85f8b35bf853964cb414a12848a4f

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
<<<<<<< HEAD
=======

...
>>>>>>> 12472e5d60b85f8b35bf853964cb414a12848a4f
