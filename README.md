# Heads-up
This was done as a school project with very limited time so just about every part of this is made to work for our use case but completely untested for everything else. This project is not being actively worked on.

# Face-Tracking-Maya
A plugin for maya tracking a face using markers and animating a 3D face-mesh based on marker positions. A C++ program using OpenCV will fetch data from the webcam, the program will then use image analysis to find tracking points. These points will be streamed as raw data to Maya using its command port which then modifies the geometry of a 3D model of a face.

![](https://github.com/oscarwestberg/Face-Tracking-Maya/raw/master/voldemort.gif)

# Members
* [Aron Tornberg](https://github.com/AronTornberg)  
* [Oscar Westberg](http://oscarwestberg.github.io)  

# How to use
* Open a face-rig in Maya with joints named as follows:

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

A ready made rig is provided in Volemort_rigged.mb

* Add corresponding markers to your own face + markers on your nose and forehead.

* Run the FaceTracker.mel script in Maya.

* Run Face-Tracking-Maya.exe

* The program assumes that you have a neutral expression when it first sees you. To set a new neutral expression press r to reset t tracking.

* Press p to toggle recording in Maya on and off.

# Resources
* [3D Model](http://tf3dm.com/3d-model/lord-voldemort-13066.html)  