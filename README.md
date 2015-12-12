# Face-Tracking-Maya
A plugin for maya tracking a face using markers and animating a 3D face-mesh based on marker positions. A C++ program using OpenCV will fetch data from the webcam, the program will then use image analysis to find tracking points. These points will be streamed as raw data to Maya using its command port which then modifies the geometry of a 3D model of a face.

# Members
* [Aron Tornberg](https://github.com/AronTornberg)  
* [Oscar Westberg](http://oscarwestberg.github.io)  

# How to use
* Open a face-rig in Maya with joints named as follows. Also add corresponding markers to your own face:
![](https://github.com/oscarwestberg/Face-Tracking-Maya/raw/master/rigreference.jpg)
	1. leftouterbrow
	2. leftinnerbrow
	3. rightinnerbrow
	4. rightouterbrow
	5. leftcheek
	6. rightcheek
	7. leftnose
	8. rightnose
	9. leftmouth
	10. upperlip
	11. rightmouth
	12. lowerlip

* Run the FaceTracker.mel script in Maya.
* Run Face-Tracking-Maya.exe
* The program assumes that you have a neutral expression when it first sees you. To set a new neutral expression press r to reset t tracking.
* Press p to toggle recording on and off.