#ifndef __FACETRACKER_H
#define __FACETRACKER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#include <String>
#include <list>

#include "TrackingData.h"

using namespace cv;

class FaceTracker
{
	public:	
		FaceTracker();
        virtual ~FaceTracker();

        void detectAndShow(Mat& frame);
        void reset();

        TrackingData getTrackingData();

	private:
        cv::CascadeClassifier face_cascade;
        cv::CascadeClassifier eyes_cascade;

        Ptr<SimpleBlobDetector> marker_detector;

        Rect savedFacePosition;
};

#endif 	/* FACETRACKER_H */
