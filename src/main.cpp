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

// Global variables

// Cascades
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "data/ojoI.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

// Frames
Mat frame;
Mat frame_gray;

// Data for previous frames
#define SIZE 5
#define MAX_DIST 1000

Point2f savedFacePosition;
vector<Point2f> savedEyePositions;

// Not currently used, may be used later to make sure a new position
// doesn't jump too far from the previous
float distance(Point p1, Rect r2) {
    Point2f p2(r2.x, r2.y);
    
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    
    return sqrt(dx * dx + dy * dy);
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
    
    face_cascade.detectMultiScale( frame_gray, faces, 1.2, 10, 0|CASCADE_SCALE_IMAGE, Size(200, 200));
    
    if (!faces.empty()) {
        savedFacePosition = Point2f(faces[0].x + faces[0].width/2, faces[0].y + faces[0].height/2);
        
        Mat faceROI = frame_gray( faces[0] );

        // Detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 2.0, 10, 0 |CASCADE_SCALE_IMAGE, Size(40, 40), Size(80,80));
        
        // Found two eyes
        if (eyes.size() > 1) {
            // eyes[0] should be the left (in image) eye
            if (eyes[0].x > eyes[1].x) {
                Rect temp = eyes[0];
                eyes[0] = eyes[1];
                eyes[1] = temp;
            }
            
            // Use saved position if eye jumps to far
            
            savedEyePositions.erase(savedEyePositions.begin());
            savedEyePositions.erase(savedEyePositions.begin());
            
            savedEyePositions.push_back(Point2f(faces[0].x + eyes[0].x + eyes[0].width/2,
                                                faces[0].y + eyes[0].y + eyes[0].height/2 ));
            savedEyePositions.push_back(Point2f(faces[0].x + eyes[1].x + eyes[1].width/2,
                                                faces[0].y + eyes[1].y + eyes[1].height/2 ));
        }
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
    int size = 4;
    
    ellipse( frame, savedFacePosition, Size( size, size ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
    ellipse( frame, savedEyePositions[0], Size( size, size ), 0, 0, 360, Scalar( 0, 255, 255 ), 4, 8, 0 );
    ellipse( frame, savedEyePositions[1], Size( size, size ), 0, 0, 360, Scalar( 0, 0, 255 ), 4, 8, 0 );
    
    /*
    // Draw face
    for ( size_t i = 0; i < faces.size(); i++ ) {
//        printf("Pos: %i, %i\n", faces[0].x, faces[0].y);
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
    }
    
    // Draw eyes
    for ( size_t j = 0; j < eyes.size(); j++ ) {
        Point eye_center( faces[0].x + eyes[j].x + eyes[j].width/2, faces[0].y + eyes[j].y + eyes[j].height/2 );
        int radius = cvRound( (eyes[j].width + eyes[j].height) * 0.25 );
        circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
    }
     */
    
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
    
    savedFacePosition = Point2f(0,0);
    savedEyePositions.push_back(Point2f(0,0));
    savedEyePositions.push_back(Point2f(0,0));
    
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