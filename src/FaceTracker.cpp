#include "FaceTracker.h"

#include <iostream>

// Not currently used
float distance(cv::Rect r1, cv::Rect r2) {
    cv::Point2f p1(r1.x, r1.y);
    cv::Point2f p2(r2.x, r2.y);
    
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    
    return sqrt(dx * dx + dy * dy);
}


FaceTracker::FaceTracker() {
    String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
    
    // Load cascade xml files
    if( !face_cascade.load( face_cascade_name ) ) perror("--(!)Error loading face cascade\n");
    savedFacePosition = Rect();
    
    //detect markers
    SimpleBlobDetector::Params params;
    params.filterByColor = false; // Blob color, 0 black, 255 white
    params.blobColor = 0;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 150;
    params.maxArea = 2000;
    params.filterByCircularity = false; // Circle has circularity of 1, square has 0.785
    params.minCircularity = 0.6;
    params.filterByConvexity = false; // How convex/concave blobs can be
    params.minConvexity = 0.6;
    params.filterByInertia = false;
    params.minInertiaRatio = 0.1;

    marker_detector = SimpleBlobDetector::create(params);
}

FaceTracker::~FaceTracker() {
}

void FaceTracker::detectAndShow(Mat& frame) {
    std::vector<Rect> faces;
    std::vector<Rect> eyes;
    Mat frame_gray = frame;
    
    // Find face
    face_cascade.detectMultiScale( frame_gray, faces, 1.2, 10, 0|CASCADE_SCALE_IMAGE, Size(200, 200));
    
    // Found face
    if (!faces.empty()) {
        // Save face position if we can't find it the next frame
        savedFacePosition = faces[0];

        // Cut out face part of image
        Rect temp = savedFacePosition;
        temp.height = min(temp.height + 100, frame.rows - temp.y);
        Mat faceROI = frame(temp).clone();
        cvtColor( faceROI, faceROI, COLOR_BGR2HSV );
        
        // Detect paper of specific color
        float H = 191 / 2.0; float S = 84 * 2.55; float V = 68 * 2.55;
        float thresholdH = 20, thresholdS = 50, thresholdV = 150;
        inRange(faceROI,
                Scalar(H - thresholdH, S - thresholdS, V - thresholdV),
                Scalar(H + thresholdH, S + thresholdS, V + thresholdV),
                faceROI);
        
        // Find markers in faceROI
        std::vector<KeyPoint> keypoints;
        marker_detector->detect(faceROI, keypoints);
        
        // Map in frame instead of faceROI
        for (int i = 0; i < keypoints.size(); i++) {
            keypoints.at(i).pt.x += savedFacePosition.x;
            keypoints.at(i).pt.y += savedFacePosition.y;
        }
        
        drawKeypoints( frame, keypoints, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
        // DEBUG: Use imshow here to se faceROI (must comment out the other imshow for this to work)
        // imshow("MyVideo", faceROI);
    }
    
    rectangle(frame, Point2f(savedFacePosition.x, savedFacePosition.y), Size(savedFacePosition.x + savedFacePosition.width, savedFacePosition.y + savedFacePosition.height), Scalar( 255, 100, 255 ));

    imshow("MyVideo", frame );
}

void FaceTracker::reset() {
}

TrackingData FaceTracker::getTrackingData() {
    TrackingData d;
    return d;
}