#pragma once

#include <fhwcg/fhwcg.h>

typedef struct {
    vec4 planeColor;
    int planeDetail;
    // noise data
    int octaves;
    float persistence;
    float frequency;
    float lacunarity;
    float scale;
    float amplitude;
    float offsetXY;
    float heightScale;
    // toggles
    bool wireframe;
    bool showHelp;
    bool windowVisible;
    bool doDisplacement;
    bool showNoiseGui;
} InputData;

void input_init(ProgContext ctx);

void input_cleanup(void);

InputData *input_getInputData(void);

void callbackFramebuffer(ProgContext ctx, int width, int height);
