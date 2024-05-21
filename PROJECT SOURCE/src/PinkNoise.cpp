//
//  PinkNoise.cpp
//  CCPY2
//
//  Created by Poliana Groth on 01/04/2024.
//

#include "PinkNoise.hpp"
#include "ofMain.h"

// Voss-McCartney algorithm for generating pink noise
// buffer of 16 values and updates it with white noise to produce pink noise
// https://www.firstpr.com.au/dsp/pink-noise/

//========================================================================
PinkNoise::PinkNoise() {
    // Initialize array
    for (int i = 0; i < NUM_BANDS; i++) {
        values[i] = 0;
        pink[i] = 0; // Initialize buffer to 0
    }
    noiseState = 0; // Initialize the noise state
}

//========================================================================
void PinkNoise::setup() {
    // Initialize the pink noise generator with random values
    for (int i = 0; i < NUM_BANDS; i++) {
        values[i] = ofRandom(-1.0, 1.0);
    }
}

//========================================================================
float PinkNoise::getNoise() {
    float white = ofRandom(-1.0, 1.0);
    
    // Calculate the next pink noise value
    float result = 0.0;
    for (int i = 0; i < NUM_BANDS; i++) {
        // Update  and combine pink noise and white noise 
        values[i] = 0.995 * values[i] + 0.03 * white;
        result += values[i]; // Accumulate the pink noise value
    }
    
    // Store the result in the pink noise buffer
    pink[noiseState] = result;
    
    // Move the state through the pink buffer
    noiseState++;
    if (noiseState >= NUM_BANDS) {
        noiseState = 0; // Wrap around if the buffer index exceeds the number of bands
    }
    
    // Return the generated pink noise value
    return pink[noiseState];
}
