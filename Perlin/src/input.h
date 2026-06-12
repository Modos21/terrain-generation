#pragma once

#include <fhwcg/fhwcg.h>

typedef struct {
    vec4 planeColor;
    int planeDetail;
    // noise data
    int octaves;
    int chunkSizeXZ;
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
    bool reloadTerrain;
    bool renderTerrain;
    bool liveUpdate;
} InputData;

void input_init(ProgContext ctx);

void input_cleanup(void);

InputData *input_getInputData(void);

void callbackFramebuffer(ProgContext ctx, int width, int height);
