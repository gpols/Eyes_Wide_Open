#define pi 3.141592653589793 // to avoid have to write every time

// variables to pass outside the shader
uniform vec2 resolution;    // Resolution of the canvas
uniform float time;         // Time since the shader started running
uniform bool eyeDetected;   // Flag for animation

// Number of octaves used for fractal Brownian motion (fbm)
// incresing the number creates a more elaborated and better resolution
#define NUM_OCTAVES 3

// Function to generate random numbers
float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

// Function to generate Perlin noise (2D)
// credits to https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float noise(vec2 p){
    // Floor the input coordinates to get integer part
    vec2 ip = floor(p);
    // Get fractional part
    vec2 u = fract(p);
    // Smoothstep for smoother noise
    u = u * u * (3.0 - 2.0 * u);
    
    // Interpolate noise values at the integer coordinates
    float res = mix(
        mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x),
        mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x), u.y);
    // Square the result for more contrast
    return res * res;
}

// Function to generate fractal Brownian motion noise
// Based on https://iquilezles.org/articles/warp/
float fbm(in vec2 x) {
    float m = 0.0;
    vec2 p = 3.0 * x;

    float w = 0.5; // Initial weight
    float s = 1.0; // Initial scale

    // Iteratively sum noise at different scales (octaves)
    for (int i = 0; i < NUM_OCTAVES; i++) {
        float n = noise(p * s); // Get noise value at scaled point
        m += w * n; // Add weighted noise value to the final result
        
        // Update point for the next octave
        // apply domain warping
        p.y -= 1.2 * (-1.0 + 2.0 * n) - 0.1 * time * w;
        s *= 2.0; // Increase the scale for each octave
        w *= 0.5; // Decrease the weight for each octave
    }

    return m;
}

// Function to convert HSV color space to RGB color space
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Calculate normalized device coordinates (uv) from pixel coordinates
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    // Generate fractal noise to create distortion effect using final noise function fbm
    float distortion = fbm(uv);

    // Offset used for computing normals
    // values of x and y
    vec2 of = vec2(0.1, 0.2);

    // Compute normal vector for lighting
    vec3 normal = normalize(vec3(fbm(uv - of.xy) - fbm(uv + of.xy), 2.0 * of.x, fbm(uv - of.yx) - fbm(uv + of.yx)));

    // Calculate base color intensity based on distortion
    float baseColourIntensity = pow(distortion, 4.0) * 6.0;
    
    // This will be used to have as a default colour
    // It takes this colour to implement and go back when eye not detected
    vec3 baseColour = vec3(0.0);

    if (eyeDetected) {
        // Compute base hue based on time
        float baseHue = mod(time * 0.01, 1.0);
        // Convert HSV color to RGB
        baseColour = hsv2rgb(vec3(baseHue, 1.0, 1.0));
    } else {
        // If eye detection is disabled, set base color to black
        baseColour = vec3(0.0);
    }

    // Adjust base color intensity
    baseColour *= 0.2 + 0.6 * baseColourIntensity;

    // Lighting also has a base colour but moves in different time for effect
    // Calculate light color intensity based on distortion
    float lightColourIntensity = pow(distortion, 3.0) * 2.0;
    vec3 lightColour = vec3(0.0);

    // Adjust light color based on whether eye detection is enabled
    if (eyeDetected) {
        // Compute light hue based on time
        float lightHue = mod(time * 0.03, 1.0);
        // Convert HSV color to RGB
        lightColour = hsv2rgb(vec3(lightHue, 1.0, 1.0));
    } else {
        // If eye detection is disabled, set light color to a default value
        lightColour = vec3(0.0);
    }

    // Adjust light color intensity
    lightColour *= 0.2 + 0.8 * lightColourIntensity;

    // Direction of the light source
    vec3 lightDirection = normalize(vec3(1.0, 0.2, 1.0));

    // Compute final color based on base color, light color, and normal vector
    baseColour *= lightColour + 1.0 * vec3(0.5, 0.0, 1.0) * clamp(0.3 + 0.7 * dot(normal, lightDirection), 0.0, 1.0); // diffuse colour
    baseColour += 1.5 * vec3(0.0, 0.5, 1.0) * pow(1.0 - normal.y, 2.0) * baseColourIntensity; // aditional colour

    // Output final color to the screen
    gl_FragColor = vec4(baseColour, 1.0);
}
