#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "Source.h"

int gridX = 250;
int gridY = 250;

//permutation table (from original ken perlin implementation)
std::vector<int> p = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36,
                       103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0,
                       26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
                       87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
                       77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
                       46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
                       187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109,
                       198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
                       255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183,
                       170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
                       172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112,
                       104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162,
                       241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106,
                       157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205,
                       93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };


static float grad(int hash, float x, float y) {
    const int h = hash & 0x3F; 
    const float u = h < 4 ? x : y; 
    const float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); 
}

static double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

//based on improved noise algorithm by ken perlin (https://mrl.cs.nyu.edu/~perlin/noise/)
double perlinNoise(double x, double y) {
    int X = (int)std::floor(x) && 255,
        Y = (int)std::floor(y) && 255;

    x -= std::floor(x);
    y -= std::floor(y);

    double u = fade(x);
    double v = fade(y);

    int A = p[X] + Y,
        AA = p[A],
        AB = p[A + 1],
        B = p[X + 1] + Y,
        BA = p[B],
        BB = p[B + 1];

    double res = lerp(v, lerp(u, grad(p[AA], x, y),
        grad(p[BA], x - 1, y)),
        lerp(u, grad(p[AB], x, y - 1),
            grad(p[BB], x - 1, y - 1)));

    return (res + 1.0) / 2.0;
}

float layeredNoise(glm::vec2 coordinates) { //used to layer multiple noise images
    float returnVal = 0.0f;
    float noiseFrequency = frequency / 10;
    float noiseAmplitude = amplitude;
    float maxAmplitude = 0.0f;

    std::srand(seed); //seeding the random function

    for (int i = 0; i < octaves; i++) {
        
        float offsetX = (std::rand() % 2001 - 1000);
        float offsetY = (std::rand() % 2001 - 1000);
        glm::vec2 seedOffset = glm::vec2(offsetX, offsetY);

        returnVal += glm::perlin(coordinates * noiseFrequency + seedOffset + noiseOffset) * noiseAmplitude;
        maxAmplitude += noiseAmplitude;

        noiseFrequency *= lacunarity;
        noiseAmplitude *= persistence; 
    }

    return returnVal;
}

std::vector<glm::vec3> mapPerlinNoise(int gridX, int gridY) {
    std::vector<glm::vec3> vertices;
    int x, y;

    for (x = 0; x < gridX; x++) {
        for (y = 0; y < gridY; y++) {
            float u = (float)x / (gridX - 1) * 10 * meshScale; // horizontal direction
            float v = (float)y / (gridY - 1) * 10 * meshScale; // vertical direction

            //sample points that are plugged into perlin noise function
            float sampleX = (float)x / noiseScale;
            float sampleY = (float)y / noiseScale;

            glm::vec2 coordinates = glm::vec2(sampleX, sampleY);

            float noiseValue = (layeredNoise(coordinates) + 1.0f) / 2.0f; // overlaying multiple perlin noise textures for detail

            float xPos = u - 0.5f;
            float zPos = v - 0.5f;

            vertices.push_back(glm::vec3(xPos, noiseValue * meshScale, zPos));
        }
    }

    return vertices;
}

#endif
