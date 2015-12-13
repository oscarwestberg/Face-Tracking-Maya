#ifndef __TRACKINGDATA_H
#define __TRACKINGDATA_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

enum Marker {
    FOREHEAD = 0,
    LEFTOUTERBROW,
    LEFTINNERBROW,
    RIGHTINNERBROW,
    RIGHTOUTERBROW,
    LEFTCHEEK,
    RIGHTCHEEK,
    LEFTNOSE,
    NOSE,
    RIGHTNOSE,
    UPPERLIP,
    LEFTMOUTH,
    RIGHTMOUTH,
    LOWERLIP,
    MARKER_COUNT
};

using namespace cv;
struct TrackingData {
    TrackingData() {

        for(int i = 0; i < MARKER_COUNT; i++) {
            markers[i] = Point2f(0,0);
        }

        timeStep = -1;
    }

    Point2f markers[MARKER_COUNT];

    float timeStep;
};

#endif 	/* TRACKINGDATA_H */
