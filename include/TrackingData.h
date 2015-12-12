#ifndef __TRACKINGDATA_H
#define __TRACKINGDATA_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
struct TrackingData {
    TrackingData() {
        leftouterbrow = Point2f(0,0);
        leftinnerbrow = Point2f(0,0);
        rightinnerbrow = Point2f(0,0);
        rightouterbrow = Point2f(0,0);

        leftcheek = Point2f(0,0);
        rightcheek = Point2f(0,0);

        leftnose = Point2f(0,0);
        rightnose = Point2f(0,0);

        upperlip = Point2f(0,0);
        leftmouth = Point2f(0,0);
        rightmouth = Point2f(0,0);
        lowerlip = Point2f(0,0);

        timeStep = -1;
    }

    Point2f leftouterbrow;
    Point2f leftinnerbrow;
    Point2f rightinnerbrow;
    Point2f rightouterbrow;

    Point2f leftcheek;
    Point2f rightcheek;

    Point2f leftnose;
    Point2f rightnose;

    Point2f upperlip;

    Point2f leftmouth;
    Point2f rightmouth;

    Point2f lowerlip;

    float timeStep;

    void draw(Mat img) {
        double fontScale = 1;
        Scalar fontColor = Scalar(0,0,0);
        putText(img, "lob", leftouterbrow, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "lib", leftinnerbrow, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "rib", rightinnerbrow, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "rob", rightouterbrow, FONT_HERSHEY_PLAIN, fontScale, fontColor);

        putText(img, "lc", leftcheek, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "rc", rightcheek, FONT_HERSHEY_PLAIN, fontScale, fontColor);

        putText(img, "ln", leftnose, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "rn", rightnose, FONT_HERSHEY_PLAIN, fontScale, fontColor);

        putText(img, "ul", upperlip, FONT_HERSHEY_PLAIN, fontScale, fontColor);

        putText(img, "lm", leftmouth, FONT_HERSHEY_PLAIN, fontScale, fontColor);
        putText(img, "rm", rightmouth, FONT_HERSHEY_PLAIN, fontScale, fontColor);

        putText(img, "ll", lowerlip, FONT_HERSHEY_PLAIN, fontScale, fontColor);
    }
};

#endif 	/* TRACKINGDATA_H */
