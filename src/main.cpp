//OpenCV webcam example from http://opencv-srf.blogspot.se/2011/09/capturing-images-videos.html

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>

#include "Socket.h"

using namespace cv;
using namespace std;

/*
 1. Run the following mel-script in Maya: ' commandPort -n ":5055"; '
 2. Run the program
 3. Maya terminal should print "Hello Maya!"
 */

// Uses SimpleBlobDetector
// Introduction can be found here http://www.learnopencv.com/blob-detection-using-opencv-python-c/
void detectAndShow(Mat frame)
{
    SimpleBlobDetector::Params params;
    params.filterByColor = true; // Blob color, 0 black, 1 white
    params.blobColor = 0.0;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 80;
    params.maxArea = 250;
    params.filterByCircularity = true; // Circle has circularity of 1, square has 0.785
    params.minCircularity = 0.7;
    params.filterByConvexity = true; // How convex/concave blobs can be
    params.minConvexity = 0.90;
    params.filterByInertia = true;
    params.minInertiaRatio = 0.1;
    
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    std::vector<KeyPoint> keypoints;
    detector->detect( frame, keypoints);
    
    Mat frame_with_keypoints;
    drawKeypoints( frame, keypoints, frame_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    
    imshow("MyVideo", frame_with_keypoints );
}


int main(int argc, char* argv[])
{
    // Create connection to Maya
    try {
        Socket mayaSocket;
        mayaSocket.connect("localhost", 5055);
        mayaSocket.send("print \"Hello Maya!\"");
    } catch (int e) {
        printf("Error setting up socket\n");
    }
    
    VideoCapture cap(0); // open the video camera no. 0
    
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    // ------------------------------------------------------
    // Main loop
    // ------------------------------------------------------
    while (1)
    {
        Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        detectAndShow(frame);
        
        // Wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(10) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    
    return 0;
}