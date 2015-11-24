# Face-Tracking-Maya
A plugin for maya tracking a face using markers and animating a 3D face-mesh based on marker positions. A C++ program using OpenCV will fetch data from the webcam, the program will then use image analysis to find tracking points. These points will be streamed as raw data to Maya using its command port which then modifies the geometry of a 3D model of a face.

# Members
* [Aron Tornberg](https://github.com/AronTornberg)  
* [Oscar Westberg](http://oscarwestberg.github.io)  

Currently this [tutorial](http://docs.opencv.org/3.0-last-rst/doc/tutorials/introduction/linux_gcc_cmake/linux_gcc_cmake.html) is implemented