#include "FaceTracker.h"

#include <iostream>

float distance(cv::Rect r1, cv::Rect r2) {
    cv::Point2f p1(r1.x, r1.y);
    cv::Point2f p2(r2.x, r2.y);
    
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    
    return sqrt(dx * dx + dy * dy);
}


FaceTracker::FaceTracker() {
    cv::String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
    cv::String eyes_cascade_name = "data/frontalEyes35x16.xml";
    cv::String noses_cascade_name = "data/Nariz.xml";
    
    // Load cascade xml files
    if( !face_cascade.load( face_cascade_name ) ) perror("--(!)Error loading face cascade\n");
    if( !eyes_cascade.load( eyes_cascade_name ) ) perror("--(!)Error loading eyes cascade\n");
    if( !noses_cascade.load( noses_cascade_name ) ) perror("--(!)Error loading eyes cascade\n");
    
    //detect markers
    SimpleBlobDetector::Params params;
    params.filterByColor = false; // Blob color, 0 black, 255 white
    params.blobColor = 0;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 2;
    params.maxArea = 100;
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
    //detect face
    Rect face;
    std::vector<Rect> faces;
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    face_cascade.detectMultiScale( frame_gray, faces, 1.3, 5, 0|CASCADE_SCALE_IMAGE, Size(30, 30));

    if (!faces.empty()) {
        face = faces[0];

        //include whole face
        float dw = face.width*0.05;
        float dh = face.height*0.1;
        face.x -= dw;
        face.y -= dh;
        face.width += 2*dw;
        face.height += 2*dh;
    } else {
        return;
    }

    Rect face_b = face;

    //save face
    old_faces.push_back(face);
    if (old_faces.size() > 4) {
        old_faces.pop_front();
    }

    //stop crash if partially out of frame
    if (face_b.x < 0) face_b.x = 0;
    if (face_b.y < 0) face_b.y = 0;
    float face_x_out = face_b.x + face_b.width - frame.size().width;
    float face_y_out = face_b.y + face_b.height - frame.size().height;
    if (face_x_out > 0) face_b.width -= face_x_out;
    if (face_y_out > 0) face_b.height -= face_y_out;

    Mat faceROI = frame(face_b).clone();
    cvtColor( faceROI, faceROI, COLOR_BGR2HSV );

    
    //klisterpapper färg
    float H = 191/2.0; float S = 84*2.55; float V = 68*2.55;
    inRange(faceROI,Scalar(H-10,S-70,V-70),Scalar(H+10,S+70,V+70),faceROI);

    
    float det_width = 280;
    float det_height = 400;
    resize(faceROI, faceROI, Size(det_width,det_height), INTER_CUBIC);

    //find markers in faceROI
    std::vector<KeyPoint> keypoints;
    // Only look for blobs in face
    marker_detector->detect(faceROI, keypoints);
    
    float scale_w = face_b.width/det_width;
    float scale_h = face_b.height/det_height;
    for (int i = 0; i < keypoints.size(); i++) {
        keypoints.at(i).pt.x *= scale_w;
        keypoints.at(i).pt.x += face_b.x;
        keypoints.at(i).pt.y *= scale_h;
        keypoints.at(i).pt.y += face_b.y;
    }
    drawKeypoints( frame, keypoints, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    //rectangle( frame, face_b.tl(), face_b.br(), Scalar( 255, 0, 255 ), 4, 8, 0 );
    imshow("MyVideo", frame );
}

void FaceTracker::reset() {
}

TrackingData FaceTracker::getTrackingData() {
    TrackingData d;
    return d;
}