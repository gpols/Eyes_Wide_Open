//
//  PinkNoise.hpp
//  CCPY2
//
//  Created by Poliana Groth on 01/04/2024.
//

#ifndef PinkNoise_hpp
#define PinkNoise_hpp

#include <stdio.h>

#pragma once

class PinkNoise {
public:
    PinkNoise();
    void setup();
    float getNoise();

private:
    static const int NUM_BANDS = 16;
    float values[NUM_BANDS];
    float pink[NUM_BANDS];
    int noiseState;
};


#endif /* PinkNoise_hpp */
