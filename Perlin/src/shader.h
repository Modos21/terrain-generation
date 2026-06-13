#pragma once

#include <fhwcg/fhwcg.h>
#include "input.h"

#define NOISE_GUI_UNIT 1
#define NOISE_TEX_UNIT 2
#define TERRAIN_TEX_UNIT 3

#define VOXEL_TEX_UNIT 4

typedef enum {
    TEX_STONE,
    TEX_GRASS,
    TEX_GRASS_SIDE,
    TEX_DIRT,
    TEX_SAND,
    TEX_WATER,
    TEX_LAVA,
    TEX_BEDROCK,
    TEX_COUNT
} VoxelTexture;

inline int voxelTexUnit(VoxelTexture tex) { return VOXEL_TEX_UNIT + tex; }

void shader_init(void);

void shader_activateSceneShader(void);

void shader_activateNoiseShader(void);

void shader_activateQuadShader(void);

void shader_activateTerrainShader(void);

void shader_reload(void);

void shader_setMVP(mat4 mvp);

void shader_setNoisePlaneData(InputData *data);

void shader_setNoiseGuiData(void);

void shader_setNormalData(mat4 nMat, mat4 mvMat, mat4 pMat, mat4 mvp);

void shader_dispatchCompute(InputData *data);

void shader_computeTerrain(InputData *data);

uint8_t *shader_getTerrainData(int size_x, int size_y, int size_z);

void shader_cleanup(void);
