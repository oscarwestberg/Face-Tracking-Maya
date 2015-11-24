# Face-Tracking-Maya
A plugin for maya tracking a face using markers and animating a 3D face-mesh based on marker positions. A C++ program using OpenCV will fetch data from the webcam, the program will then use image analysis to find tracking points. These points will be streamed as raw data to Maya using its command port which then modifies the geometry of a 3D model of a face.
