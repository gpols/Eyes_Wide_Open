#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxMaxim.h"
#include "maxiGrains.h"
#include "maximilian.h"
#include "PinkNoise.hpp"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void exit() override;
    void update() override;
    void draw() override;
    void audioOut(float *output, int buffer_size, int channels) override;

    maxiOsc m_osc_1;
    maxiOsc m_osc_2;
    unsigned int m_sample_rate;
    unsigned int m_buffer_size;
    
    
private:
    ofShader shader;
    ofVideoGrabber camera;
    ofxCvColorImage colour;
    ofxCvGrayscaleImage gray;
    ofxCvHaarFinder haar;
    
    bool eyeDetected = false; // Flag indicating whether an eye is detected
    float lastDetection; // Time since last eye detection
    float lastDetectionTime; // Timestamp of last eye detection
    float detectionResetTime = 8.0; // Time threshold for resetting eye detection
      
    PinkNoise pinkNoise; // Pink noise generator object


};
