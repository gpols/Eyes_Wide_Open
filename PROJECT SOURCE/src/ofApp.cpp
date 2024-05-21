#include "ofApp.h" 
//========================================================================
void ofApp::setup() {
    shader.load("Noise"); // Load the shader
    camera.setup(ofGetWidth(), ofGetHeight()); // Setup the camera
    colour.allocate(camera.getWidth(), camera.getHeight()); // Allocate memory for the color image
    gray.allocate(camera.getWidth(), camera.getHeight()); // Allocate memory for the grayscale image
    haar.setup("haarcascade_eye.xml"); // Setup the Haar cascade classifier for eye detection
    haar.setScaleHaar(2); // Set the scale of the Haar cascade
    
    // Setup audio
    m_sample_rate = 44100; // Set the audio sample rate
    m_buffer_size = 512; // Set the audio buffer size
    maxiSettings::setup(m_sample_rate, 2, m_buffer_size); // Setup maxi settings
    ofSoundStreamSetup(2, 0, m_sample_rate, m_buffer_size, 4); // Setup the sound stream with specified parameters
    pinkNoise.setup(); // Setup the pink noise generator
}

//========================================================================
void ofApp::exit() {
    ofSoundStreamClose();
}

//========================================================================
void ofApp::update() {
    camera.update(); // Update the camera
    
    if (camera.isFrameNew()) { // Check if the camera has a new frame
        colour.setFromPixels(camera.getPixels()); // Set the color image from camera pixels
        gray = colour; // Convert the color image to grayscale
        haar.findHaarObjects(gray); // Find Haar objects (eyes) in the grayscale image
        
        if (!haar.blobs.empty()) { // If eyes are detected
            eyeDetected = true; // Set the flag to true
            lastDetectionTime = ofGetElapsedTimef(); // Update the last detection time
            
        } else { // If no eyes are detected
            if (ofGetElapsedTimef() - lastDetectionTime > detectionResetTime) {
                eyeDetected = false; // Reset the flag after a certain time if no detection
            }
        }
        
        lastDetection = ofGetElapsedTimef() - lastDetectionTime; // Calculate time since last detection
    }
}

//========================================================================
void ofApp::draw() {
    
    shader.begin();
    
    shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight()); // Set shader uniform for resolution
    shader.setUniform1f("time", ofGetElapsedTimef()); // Set shader uniform for time
    shader.setUniform1i("eyeDetected", eyeDetected && lastDetection ? 1 : 0); // Set shader uniform for eye detection
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight()); // Draw a rectangle covering the screen
    
    shader.end();
}

//========================================================================
void ofApp::audioOut(float *output, int buffer_size, int channels) {
    
    float pink;
    
    for (unsigned int i = 0; i < buffer_size; ++i) { // Iterate over each sample in the buffer
        
        if (eyeDetected) { 
            
            // Sound when eye is detected
            double osc_value = m_osc_1.sinewave(5 + (m_osc_2.sinewave(50) * 10)); // Generate sine wave with modulation
            
            output[i * channels] = osc_value; // Set left output
            output[i * channels + 1] = osc_value; // Set right output
            
        } else {
            
            // Sound when eye is not detected
            pink = 0.002 * pinkNoise.getNoise(); // Generate pink noise
            output[i * channels] = pink; // Set left output
            output[i * channels + 1] = pink; // Set right output
        }
    }
}
