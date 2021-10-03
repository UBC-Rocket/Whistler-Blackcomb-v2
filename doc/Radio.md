# Radio

## Project setup

All the radio code is taken from [this](https://github.com/digidotcom/xbee_ansic_library) library, which is a portable ANSIC C library for xbee radios. Our usage of this library essentially consistutes a port for our specific hardware/hal. The code for the library is in the [xbee](../common/xbee) folder. To successfully port this library to our application, several changes had to be made. 

1. Target declaration

The build tool must first declare the target, and this is done through a preprocessor define statement. For x86 this is done in the CMakeLists.txt in the main directory, whereas in MCUXpresso it is done through the UI, through Project->Properties->C/C++ General->Paths and Symbols->Symbols. 

2. Removal of uncessesary files

The library supports functionality not needed by us, such as over the air firmware updating and a few others. Since we're not interested in any of this and ports need to to do extra work to make this work, all of the files relating to OTA and commission were removed. This approach was taken from the mbed port, so hopefully this doens't break anything too badly. In addition, we have to implement ```xbee_frame_handlers``` as the frame handler. The way this is set up (using external variables on the assumption the user will implement them) is pretty weird, but that's the way it works. 

2. Writing of the port

To write the port, many functions had to be implemented that used platform specific code. To see what had to be implemented everything was based on the posix port [here](../common/xbee/ports/posix). Our port is called whistler and can be seen [here](../common/xbee/ports/whistler). 

## Utility wrapper

To make calling radio functions easier for our specific use case, I wrote a few wrapper functions in [radio.h](../common/include/radio.h). See there for documentation on these functions. 
