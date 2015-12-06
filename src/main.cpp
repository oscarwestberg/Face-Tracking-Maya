//OpenCV webcam example from http://opencv-srf.blogspot.se/2011/09/capturing-images-videos.html

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <queue>
#include <ctime>

#include "Socket.h"

using namespace cv;
using namespace std;

/*
 1. Run the following mel-script in Maya: ' commandPort -n ":5055"; '
 2. Run the program
 3. Maya terminal should print "Hello Maya!"
 */


// Global variables

// Cascades
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "data/frontalEyes35x16.xml";
String noses_cascade_name = "data/Nariz.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier noses_cascade;

// Frames
Mat frame;
Mat frame_gray;

// Data for previous frames
#define SIZE 5
#define MAX_DIST 200

Rect faceData;
Rect eyeData;
Rect noseData;

float distance(Rect r1, Rect r2) {
    Point2f p1(r1.x, r1.y);
    Point2f p2(r2.x, r2.y);
    
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    
    return sqrt(dx * dx + dy * dy);
}

void handleData(std::vector<Rect> &rects, Rect &data) {

    // If the vector is empty, use an old value
    if (rects.empty()) {
        // if faceData has been initialized
        if (data.x > 0) {
            printf("There is data: %i, %i\n", data.x, data.y);
            rects[0] = data;
            printf("1\n");
        } else {
            printf("Nothing exists\n");
        }
    } else {
        // Pick the best value among the ones avaiable
        printf("Saving: %i, %i\n", rects[0].x, rects[0].y);
        data = rects[0];
        printf("2\n");
    }
    
    // Is there a point close to the estimate?
}

// Uses SimpleBlobDetector
// Introduction can be found here http://www.learnopencv.com/blob-detection-using-opencv-python-c/
void detectAndShow()
{
    // ------------------------------------------------------
    // Detect facial features
    // ------------------------------------------------------
    std::vector<Rect> faces;
    std::vector<Rect> eyes;
    std::vector<Rect> noses;
    
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    
    face_cascade.detectMultiScale( frame_gray, faces, 1.3, 5, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
    handleData(faces, faceData);
    
    if (!faces.empty()) {
        Mat faceROI = frame_gray( faces[0] );

        // Detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 8, 0 |CASCADE_SCALE_IMAGE, Size(20, 20) );
    //    handleData(eyes, eyeData);
        
        // Detect nose
        noses_cascade.detectMultiScale( faceROI, noses, 1.1, 10, 0 |CASCADE_SCALE_IMAGE, Size(20, 20) );
    //    handleData(noses, noseData);
    }


    // ------------------------------------------------------
    // Detect blobs
    // ------------------------------------------------------
    /*
    SimpleBlobDetector::Params params;
    params.filterByColor = false; // Blob color, 0 black, 255 white
    params.blobColor = 2;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 80;
    params.maxArea = 600;
    params.filterByCircularity = true; // Circle has circularity of 1, square has 0.785
    params.minCircularity = 0.6;
    params.filterByConvexity = false; // How convex/concave blobs can be
    params.minConvexity = 0.6;
    params.filterByInertia = false;
    params.minInertiaRatio = 0.1;
    
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    std::vector<KeyPoint> keypoints;
    // Only look for blobs in face
    detector->detect(faceROI, keypoints);
    */
    
    
    
    // ------------------------------------------------------
    // Draw image and locations
    // ------------------------------------------------------
    // Draw face
    for ( size_t i = 0; i < faces.size(); i++ ) {
        printf("Pos: %i, %i\n", faces[0].x, faces[0].y);
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
    }
    
    // Draw eyes
    for ( size_t j = 0; j < eyes.size(); j++ ) {
        Point eye_center( faces[0].x + eyes[j].x + eyes[j].width/2, faces[0].y + eyes[j].y + eyes[j].height/2 );
        int radius = cvRound( (eyes[j].width + eyes[j].height) * 0.25 );
        circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
    }
    
    // Draw nose
    for ( size_t j = 0; j < noses.size(); j++ ) {
        Point nose_center( faces[0].x + noses[j].x + noses[j].width/2, faces[0].y + noses[j].y + noses[j].height/2 );
        int radius = cvRound( (noses[j].width + noses[j].height) * 0.25 );
        circle( frame, nose_center, radius, Scalar( 0, 255, 0 ), 4, 8, 0 );
    }
    
    /*
    // Draw blobs
    for (int i = 0; i < keypoints.size(); i++) {
        keypoints.at(i).pt.x += faces[0].x;
        keypoints.at(i).pt.y += faces[0].y;
    }
    
    drawKeypoints( frame, keypoints, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
     */
    
    imshow("MyVideo", frame );
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
    
    // Load cascade xml files
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
    if( !noses_cascade.load( noses_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
    
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
    
    // Steup FPS calculation
    int framecount = 0;
    std::clock_t start;
    double duration;
    start = std::clock();
    
    // ------------------------------------------------------
    // Main loop
    // ------------------------------------------------------
    while (1)
    {
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        detectAndShow();
        
        // Wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(10) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
        
        // Calculate FPS
        framecount++;
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        if (duration > 1.0) {
            printf("FPS: %i\n", framecount);
            start = std::clock();
            framecount = 0;
        }
    }
    
    return 0;
}