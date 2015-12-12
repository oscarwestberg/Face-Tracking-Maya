#include "FaceTracker.h"

#include <iostream>

#define ROUND_STICKER_MARKER 1

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
#if ROUND_STICKER_MARKER
    SimpleBlobDetector::Params params;
    params.filterByColor = true; // Blob color, 0 black, 255 white
    params.blobColor = 255;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 50;
    params.maxArea = 2000;
    params.filterByCircularity = true; // Circle has circularity of 1, square has 0.785
    params.minCircularity = 0.4;
    params.filterByConvexity = false; // How convex/concave blobs can be
    params.minConvexity = 0.6;
    params.filterByInertia = false;
    params.minInertiaRatio = 0.1;
    H = 176 / 2.0; S = 57 * 2.55; V = 75 * 2.55; //teal markers
#else
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
    H = 191 / 2.0; S = 84 * 2.55; V = 68 * 2.55; //glossy paper
#endif

    marker_detector = SimpleBlobDetector::create(params);

    face_rest_captured = false;
}

FaceTracker::~FaceTracker() {
}

bool keySortSmallX(KeyPoint k1, KeyPoint k2) {
    return k1.pt.x < k2.pt.x;
}

bool keySortSmallY(KeyPoint k1, KeyPoint k2) {
    return k1.pt.y < k2.pt.y;
}

bool FaceTracker::detectAndShow(Mat& frame) {
    //###########
    //DETECT FACE
    //###########
    std::vector<Rect> faces;
    Mat frame_gray = frame;
    
    face_cascade.detectMultiScale( frame_gray, faces, 1.2, 10, 0|CASCADE_SCALE_IMAGE, Size(200, 200));
    
    //############
    //FILTER IMAGE
    //############
    if (faces.empty()) {
        return false;
    }


    rectangle(frame, Point2f(savedFacePosition.x, savedFacePosition.y), Size(savedFacePosition.x + savedFacePosition.width, savedFacePosition.y + savedFacePosition.height), Scalar( 255, 100, 255 ));
    // Save face position if we can't find it the next frame
    savedFacePosition = faces[0];

    // Cut out face part of image
    Rect temp = savedFacePosition;
    temp.height = min(temp.height + 100, frame.rows - temp.y);
    Mat faceROI = frame(temp).clone();
    cvtColor( faceROI, faceROI, COLOR_BGR2HSV );
        
    // Detect paper of specific color
    float thresholdH = 15, thresholdS = 50, thresholdV = 150;
        
    inRange(faceROI,
            Scalar(H - thresholdH, S - thresholdS, V - thresholdV),
            Scalar(H + thresholdH, S + thresholdS, V + thresholdV),
            faceROI);

        
    //uniform size
    float det_width = 300;
    float det_height = 500;
    resize(faceROI, faceROI, Size(det_width,det_height), INTER_CUBIC);
    blur(faceROI, faceROI, Size(10,10));
    threshold(faceROI,faceROI,50,255,THRESH_BINARY);


    //##############
    //DETECT MARKERS
    //##############

    // Find markers in faceROI
    std::vector<KeyPoint> keypoints;
    marker_detector->detect(faceROI, keypoints);
        
    // Map in frame instead of faceROI
    float scale_w = temp.width/det_width;
    float scale_h = temp.height/det_height;
    for (int i = 0; i < keypoints.size(); i++) {
        keypoints.at(i).pt.x *= scale_w;
        keypoints.at(i).pt.x += savedFacePosition.x;
        keypoints.at(i).pt.y *= scale_h;
        keypoints.at(i).pt.y += savedFacePosition.y;
    }
        
    drawKeypoints( frame, keypoints, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    imshow("Filtered", faceROI);

    //should detect exactly 12 points
    if(keypoints.size() != 12) {
        return false;
    }

    //which keypoint is which?
    int keyIndex = 0;
    std::sort(keypoints.begin(), keypoints.end(), keySortSmallY);
    
    TrackingData *face_data;
    if (face_rest_captured) {
        face_data = &face_move_data;
    } else {
        face_data = &face_rest_data;
        face_rest_captured = true;
    }

    //brows
    std::sort(keypoints.begin(), keypoints.begin()+4, keySortSmallX);
    face_data->leftouterbrow = keypoints[keyIndex++].pt - face_rest_data.leftouterbrow;
    face_data->leftinnerbrow = keypoints[keyIndex++].pt - face_rest_data.leftinnerbrow;
    face_data->rightinnerbrow = keypoints[keyIndex++].pt - face_rest_data.rightinnerbrow;
    face_data->rightouterbrow = keypoints[keyIndex++].pt - face_rest_data.rightouterbrow;

    //cheeks
    std::sort(keypoints.begin()+4, keypoints.begin()+6, keySortSmallX);
    face_data->leftcheek = keypoints[keyIndex++].pt - face_rest_data.leftcheek;
    face_data->rightcheek = keypoints[keyIndex++].pt - face_rest_data.rightcheek;

    //nose and upper lip
    std::sort(keypoints.begin()+6, keypoints.begin()+9, keySortSmallX);
    face_data->leftnose = keypoints[keyIndex++].pt - face_rest_data.leftnose;
    face_data->upperlip = keypoints[keyIndex++].pt - face_rest_data.upperlip;
    face_data->rightnose = keypoints[keyIndex++].pt - face_rest_data.rightnose;

    //sides of mouth and lower lip
    std::sort(keypoints.begin()+9, keypoints.end(), keySortSmallX);
    face_data->leftmouth = keypoints[keyIndex++].pt - face_rest_data.leftmouth;
    face_data->lowerlip = keypoints[keyIndex++].pt - face_rest_data.lowerlip;
    face_data->rightmouth = keypoints[keyIndex++].pt - face_rest_data.rightmouth;

    face_rest_data.draw(frame);
    return true;
}

void FaceTracker::reset() {
    face_rest_captured = false;
    face_move_data = TrackingData();
    face_rest_data = TrackingData();
}

TrackingData& FaceTracker::getTrackingData() {
    return face_move_data;
}