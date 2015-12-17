#include "FaceTracker.h"

#include <iostream>

#define ROUND_STICKER_MARKER 1
#define MAX_DIST 20

float distance(cv::Point2f p1, cv::Point2f p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;

    return sqrt(dx * dx + dy * dy);
}


FaceTracker::FaceTracker() {
    String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
    hasFoundFace = false;

    // Load cascade xml files
    if( !face_cascade.load( face_cascade_name ) ) perror("--(!)Error loading face cascade\n");
    savedFacePosition = Rect();
    
    //detect markers
#if ROUND_STICKER_MARKER
    SimpleBlobDetector::Params params;
    params.filterByColor = true; // Blob color, 0 black, 255 white
    params.blobColor = 255;
    params.filterByArea = true; // How many pixels min/max a blob can have
    params.minArea = 100;
    params.maxArea = 1000;
    params.filterByCircularity = true; // Circle has circularity of 1, square has 0.785
    params.minCircularity = 0.4;
    params.filterByConvexity = false; // How convex/concave blobs can be
    params.minConvexity = 0.6;
    params.filterByInertia = false;
    params.minInertiaRatio = 0.1;
    H = 176 / 2.0; S = 57 * 2.55; V = 75 * 2.55; //teal markers
    thresholdH = 15; thresholdS = 50; thresholdV = 150;
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
    thresholdH = 15; thresholdS = 50; thresholdV = 250;
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
    
    // ###########
    // DETECT FACE
    // ###########
    std::vector<Rect> faces;
    Mat frame_gray = frame;
    
    face_cascade.detectMultiScale( frame_gray, faces, 1.2, 10, 0|CASCADE_SCALE_IMAGE, Size(200, 200));
    
    // ############
    // FILTER IMAGE
    // ############
    if (!faces.empty()) {
        savedFacePosition = faces[0];
        hasFoundFace = true;
    } else if (!hasFoundFace) {
        return false;
    }

    rectangle(frame, Point2f(savedFacePosition.x, savedFacePosition.y), Size(savedFacePosition.x + savedFacePosition.width, savedFacePosition.y + savedFacePosition.height), Scalar( 255, 100, 255 ));

    // Cut out face part of image
    Rect temp = savedFacePosition;
    temp.height = min(temp.height + 50, frame.rows - temp.y);
    Mat faceROI = frame(temp).clone();
    cvtColor( faceROI, faceROI, COLOR_BGR2HSV );
        
    // Detect paper of specific color
        
    inRange(faceROI,
            Scalar(H - thresholdH, S - thresholdS, V - thresholdV),
            Scalar(H + thresholdH, S + thresholdS, V + thresholdV),
            faceROI);
        
    // Uniform size
    float det_width = 300;
    float det_height = 500;
    resize(faceROI, faceROI, Size(det_width,det_height), INTER_CUBIC);
    blur(faceROI, faceROI, Size(10,10));
    threshold(faceROI,faceROI,50,255,THRESH_BINARY);


    // ##############
    // DETECT MARKERS
    // ##############

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
        
        /*
        Point2f pt = keypoints.at(i).pt;
        pt.x *= scale_w;
        pt.x += savedFacePosition.x;
        pt.y *= scale_h;
        pt.y += savedFacePosition.y;
        float size_w = keypoints.at(i).size * scale_w/2;
        float size_h = keypoints.at(i).size * scale_h/2;
        ellipse(frame,pt,Size(size_h,size_w),0,0,360,Scalar(0,0,255));
        */
        
    }
    
    
    // If keypoints has the wrong size, try to correct
    if (keypoints.size() != MARKER_COUNT) {
        //return false; //TEMPORARY FIX


        // There are no saved keypoints either
        if (savedKeypoints.empty()) {
            return false;
        }

        std::vector<KeyPoint> tempKeypoints;
        
        // Loop through saved keypoints
        for (int i = 0; i < MARKER_COUNT; i++) {
            KeyPoint tempPoint = savedKeypoints[i];
            float closestDist = 99999.0;
            
            // Loop through all found keypoints
            for (int j = 0; j < keypoints.size(); j++) {
                float dist = distance(tempPoint.pt, keypoints[j].pt);
                
                // If we found a better candidate, change tempPoint to this
                if (dist < MAX_DIST && dist < closestDist) {
                    closestDist = dist;
                    tempPoint = keypoints[j];
                }
            }
            
            tempKeypoints.push_back(tempPoint);
        }
        
        // Clear keypoints
        // Fill keypoints with values from tempKeypoints
        keypoints.clear();
        for (int i = 0; i < MARKER_COUNT; i++) {
            keypoints.push_back(tempKeypoints[i]);
        }
    }
    
    // Sort keypoints to correct order
    std::sort(keypoints.begin(), keypoints.end(), keySortSmallY);
    std::sort(keypoints.begin()+1, keypoints.begin()+5, keySortSmallX);
    std::sort(keypoints.begin()+5, keypoints.begin()+7, keySortSmallX);
    std::sort(keypoints.begin()+7, keypoints.begin()+10, keySortSmallX);
    std::sort(keypoints.begin()+10, keypoints.end(), keySortSmallX);
    std::sort(keypoints.begin()+11, keypoints.begin()+13, keySortSmallY);
    
    // Weight keypoints with saved ones
    if (!savedKeypoints.empty()) {
        for (int i = 0; i < keypoints.size(); i++) {
            keypoints[i].pt.x = keypoints[i].pt.x * 0.7 + savedKeypoints[i].pt.x * 0.3;
            keypoints[i].pt.y = keypoints[i].pt.y * 0.7 + savedKeypoints[i].pt.y * 0.3;
        }
    }
    
    // Save keypoints
    savedKeypoints.clear();
    for (int i = 0; i < MARKER_COUNT; i++) {
        savedKeypoints.push_back(keypoints[i]);
    }
    
    // Display keypoints in frame
    drawKeypoints( frame, keypoints, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    imshow("Filtered", faceROI);
    
    //which keypoint is which?
    int keyIndex = 0;
    TrackingData face_data;

    //forehead
    face_data.markers[FOREHEAD] = keypoints[keyIndex++].pt;

    //brows
    face_data.markers[LEFTOUTERBROW] = keypoints[keyIndex++].pt;
    face_data.markers[LEFTINNERBROW] = keypoints[keyIndex++].pt;
    face_data.markers[RIGHTINNERBROW] = keypoints[keyIndex++].pt;
    face_data.markers[RIGHTOUTERBROW] = keypoints[keyIndex++].pt;

    //cheeks
    face_data.markers[LEFTCHEEK] = keypoints[keyIndex++].pt;
    face_data.markers[RIGHTCHEEK] = keypoints[keyIndex++].pt;

    //nose
    face_data.markers[LEFTNOSE] = keypoints[keyIndex++].pt;
    face_data.markers[NOSE] = keypoints[keyIndex++].pt;
    face_data.markers[RIGHTNOSE] = keypoints[keyIndex++].pt;

    //mouth
    face_data.markers[LEFTMOUTH] = keypoints[keyIndex++].pt;
    face_data.markers[UPPERLIP] = keypoints[keyIndex++].pt;
    face_data.markers[LOWERLIP] = keypoints[keyIndex++].pt;
    face_data.markers[RIGHTMOUTH] = keypoints[keyIndex++].pt;

    //if no rest face, save this one, with forehead at (0,0)
    if (!face_rest_captured) {
        for (int i = 0; i < MARKER_COUNT; i++) {
            face_rest_data.markers[i] = face_data.markers[i] - face_data.markers[FOREHEAD];
            face_prev_data.markers[i] = face_data.markers[i];
        }
        face_rest_captured = true;
        return true;
    }

    //if to big difference from previous frame, skip
    for (int i = 0; i < MARKER_COUNT; i++) {
        if (distance(face_prev_data.markers[i], face_data.markers[i]) > 100) {
            return false;
        }
        face_prev_data.markers[i] = face_data.markers[i];
    }

    //normalized forehead to nose vector in rest state
    float rest_dist = distance(face_rest_data.markers[FOREHEAD], face_rest_data.markers[NOSE]);
    Point2f fn_v_r = (face_rest_data.markers[FOREHEAD] - face_rest_data.markers[NOSE])/rest_dist;

    //normalied forehead to nose vector in current state
    float curr_dist = distance(face_data.markers[FOREHEAD], face_data.markers[NOSE]);
    Point2f fn_v_c = (face_data.markers[FOREHEAD] - face_data.markers[NOSE])/curr_dist;

    line(frame, face_data.markers[FOREHEAD], face_data.markers[NOSE], Scalar(255,255,255));
    line(frame, face_rest_data.markers[FOREHEAD] + face_data.markers[FOREHEAD], face_rest_data.markers[NOSE] + face_data.markers[FOREHEAD], Scalar(0,255,255));

    //face tilt
    float cosTheta = fn_v_r.x*fn_v_c.x + fn_v_r.y*fn_v_c.y;
    float sinTheta = sqrt(1.0f - cosTheta*cosTheta);
    if(fn_v_c.x < fn_v_r.x) sinTheta = -sinTheta;

    float trans_scale = curr_dist/rest_dist;

    //rotate and get difference to modified rest data
    for (int i = 0; i < MARKER_COUNT; i++) {
        Point2f rest = face_rest_data.markers[i] * trans_scale;
        rest = Point2f(rest.x*cosTheta - rest.y*sinTheta, rest.x*sinTheta + rest.y*cosTheta);

        Point2f curr = face_data.markers[i] - face_data.markers[FOREHEAD];

        face_move_data.markers[i] = (curr - rest)/curr_dist;

        //draw
        Point2f forehead = face_data.markers[FOREHEAD];
        Point2f pt1 = rest + forehead;
        Point2f pt2 = curr + forehead;
        line(frame, pt1, pt2, Scalar(0,255,0));
        circle(frame, pt1, 2, Scalar(255,0,0));
        circle(frame, pt2, 2, Scalar(255,255,0));
    }

    return true;
}

void FaceTracker::reset() {
    face_rest_captured = false;
    savedFacePosition = Rect();
    hasFoundFace = false;
    savedKeypoints.clear();
    face_move_data = TrackingData();
    face_rest_data = TrackingData();
}

TrackingData& FaceTracker::getTrackingData() {
    return face_move_data;
}