#ifndef __TRACKINGDATA_H
#define __TRACKINGDATA_H

struct vec2 {
    float x;
    float y;
};

struct TrackingData {
    vec2 leftouterbrow;
    vec2 leftinnerbrow;
    vec2 rightinnerbrow;
    vec2 rightouterbrow;

    vec2 leftcheek;
    vec2 rightcheek;
    
    vec2 leftnose;
    vec2 middlenose;
    vec2 rightnose;

    vec2 leftupperlip;
    vec2 middleupperlip;
    vec2 rightupperlip;

    vec2 leftlowerlip;
    vec2 middlelowerlip;
    vec2 rightlowerlip;

    vec2 leftjaw;
    vec2 rightjaw;
    vec2 chin;

    float timeStep;
};

#endif 	/* TRACKINGDATA_H */
