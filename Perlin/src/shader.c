#include <time.h>
#include "shader.h"
#include "mesh.h"

static Shader* g_noiseShader = NULL;
static Shader* g_quadShader = NULL;
static Shader* g_computeShader = NULL;
static Shader* g_terrainCompShader = NULL;
static Shader* g_terrainShader = NULL;

static Shader* g_activeShader = NULL;

static GLuint g_computeTexture = 0;
static ivec3 texture_res = {512, 512, 1};

static GLuint g_terrainTexture = 0;
static ivec3 terrain_res = {CHUNK_XZ, CHUNK_Y, CHUNK_XZ};

static GLuint g_voxelTexIDs[TEX_COUNT] = {0};

static GLuint createTexture2D(GLint internalFormat, const ivec2 size) {
    GLuint texId;
    glCreateTextures(GL_TEXTURE_2D, 1, &texId);

    glTextureStorage2D(texId, 1, internalFormat, size[0], size[1]);

    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texId;
}

static GLuint createTerrainTexture(int size_x, int size_y, int size_z) {
    GLuint texId;
    glCreateTextures(GL_TEXTURE_3D, 1, &texId);

    glTextureStorage3D(texId, 1, GL_R8UI, size_x, size_y, size_z);

    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texId;
}

static void loadTexture2D(VoxelTexture tex, const char *const filepath) {
    g_voxelTexIDs[tex] = texture_loadTexture(filepath, GL_CLAMP_TO_BORDER);
    glBindTextureUnit(voxelTexUnit(tex), g_voxelTexIDs[tex]);
    glTextureParameteri(g_voxelTexIDs[tex], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(g_voxelTexIDs[tex], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

/**
 * Loads in the textures and creates texture objects for them
 */
static void loadVoxelTextures(void) {
    loadTexture2D(TEX_STONE, RESOURCE_PATH "tex/stone.png");
    loadTexture2D(TEX_GRASS, RESOURCE_PATH "tex/grass.png");
    loadTexture2D(TEX_GRASS_SIDE, RESOURCE_PATH "tex/grass_side.png");
    loadTexture2D(TEX_DIRT, RESOURCE_PATH "tex/dirt.png");
    loadTexture2D(TEX_SAND, RESOURCE_PATH "tex/sand.png");
    loadTexture2D(TEX_WATER, RESOURCE_PATH "tex/water.png");
    loadTexture2D(TEX_LAVA, RESOURCE_PATH "tex/lava.png");
    loadTexture2D(TEX_BEDROCK, RESOURCE_PATH "tex/bedrock.png");
}

static void uploadVoxelTextures(void) {
    if (!g_terrainShader) return;

    shader_setInt(g_terrainShader, "u_texStone", voxelTexUnit(TEX_STONE));
    shader_setInt(g_terrainShader, "u_texGrass", voxelTexUnit(TEX_GRASS));
    shader_setInt(g_terrainShader, "u_texGrassSide", voxelTexUnit(TEX_GRASS_SIDE));
    shader_setInt(g_terrainShader, "u_texDirt", voxelTexUnit(TEX_DIRT));
    shader_setInt(g_terrainShader, "u_texSand", voxelTexUnit(TEX_SAND));
    shader_setInt(g_terrainShader, "u_texWater", voxelTexUnit(TEX_WATER));
    shader_setInt(g_terrainShader, "u_texLava", voxelTexUnit(TEX_LAVA));
    shader_setInt(g_terrainShader, "u_texBedrock", voxelTexUnit(TEX_BEDROCK));
}

void shader_init(void) {
    loadVoxelTextures();

    g_noiseShader = shader_createVeFrShader("Noise",
                                            RESOURCE_PATH "shader/noise.vert", RESOURCE_PATH "shader/noise.frag");

    if (!g_noiseShader) {
        exit(EXIT_FAILURE);
    }

    g_quadShader = shader_createVeFrShader("Noise Quad",
                                           RESOURCE_PATH "shader/quad.vert", RESOURCE_PATH "shader/quad.frag");

    if (!g_quadShader) {
        exit(EXIT_FAILURE);
    }

    g_terrainShader = shader_createVeFrShader("Chunk",
                                              RESOURCE_PATH "shader/terrain.vert", RESOURCE_PATH "shader/terrain.frag");

    if (!g_terrainShader) {
        exit(EXIT_FAILURE);
    }

    g_computeShader = shader_createShader();
    shader_attachShaderFile(g_computeShader, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/fbm.comp");

    if (!shader_buildShader("Compute", &g_computeShader)) {
        exit(EXIT_FAILURE);
    }

    g_computeTexture = createTexture2D(GL_RGBA32F, texture_res);
    glBindTextureUnit(NOISE_TEX_UNIT, g_computeTexture);
    glBindImageTexture(NOISE_TEX_UNIT, g_computeTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    g_terrainCompShader = shader_createShader();
    shader_attachShaderFile(g_terrainCompShader, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/terrain.comp");

    if (!shader_buildShader("Terrain", &g_terrainCompShader)) {
        exit(EXIT_FAILURE);
    }

    g_terrainTexture = createTerrainTexture(CHUNK_XZ, CHUNK_Y, CHUNK_XZ);
    glBindTextureUnit(TERRAIN_TEX_UNIT, g_terrainTexture);
    glBindImageTexture(TERRAIN_TEX_UNIT, g_terrainTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

    uploadVoxelTextures();
}

void shader_reload(void) {
    loadVoxelTextures();

    Shader* newNoise = shader_createVeFrShader("Noise",
                                               RESOURCE_PATH "shader/noise.vert", RESOURCE_PATH "shader/noise.frag");

    if (newNoise) {
        shader_deleteShader(&g_noiseShader);
        g_noiseShader = newNoise;
    } else {
        shader_deleteShader(&newNoise);
    }

    Shader* newQuad = shader_createVeFrShader("Noise Quad",
                                              RESOURCE_PATH "shader/quad.vert", RESOURCE_PATH "shader/quad.frag");

    if (newQuad) {
        shader_deleteShader(&g_quadShader);
        g_quadShader = newQuad;
    } else {
        shader_deleteShader(&newQuad);
    }

    Shader* newChunk = shader_createVeFrShader("Chunk",
                                               RESOURCE_PATH "shader/terrain.vert",
                                               RESOURCE_PATH "shader/terrain.frag");

    if (newChunk) {
        shader_deleteShader(&g_terrainShader);
        g_terrainShader = newChunk;
    } else {
        shader_deleteShader(&newChunk);
    }

    Shader* newComp = shader_createShader();
    shader_attachShaderFile(newComp, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/fbm.comp");

    if (shader_buildShader("Compute", &newComp) && newComp) {
        shader_deleteShader(&g_computeShader);
        g_computeShader = newComp;
    } else {
        shader_deleteShader(&newComp);
    }

    Shader* newTerrain = shader_createShader();
    shader_attachShaderFile(newTerrain, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/terrain.comp");

    if (shader_buildShader("Terrain", &newTerrain) && newTerrain) {
        shader_deleteShader(&g_terrainCompShader);
        g_terrainCompShader = newTerrain;
    } else {
        shader_deleteShader(&newTerrain);
    }

    uploadVoxelTextures();
}

void shader_activateNoiseShader(void) {
    shader_useShader(g_noiseShader);
    g_activeShader = g_noiseShader;
}

void shader_activateQuadShader(void) {
    shader_useShader(g_quadShader);
    g_activeShader = g_quadShader;
}

void shader_activateTerrainShader(void) {
    shader_useShader(g_terrainShader);
    g_activeShader = g_terrainShader;
}

void shader_setMVP(mat4 mvp) {
    shader_setMat4(g_activeShader, "u_mvpMatrix", (mat4*)mvp);
}

void shader_setNoisePlaneData(InputData* data) {
    shader_setInt(g_noiseShader, "u_noiseTex", NOISE_TEX_UNIT);
    shader_setBool(g_noiseShader, "u_doDisplacement", data->doDisplacement);
    shader_setFloat(g_noiseShader, "u_heightScale", data->heightScale);
    shader_setVec4(g_noiseShader, "u_planeColor", (vec4*)data->planeColor);
}

void shader_setNoiseGuiData(void) {
    shader_setInt(g_quadShader, "u_noiseTex", NOISE_TEX_UNIT);
}

void shader_dispatchCompute(InputData* data) {
    shader_useShader(g_computeShader);
    shader_setFloat(g_computeShader, "u_persistence", data->persistence);
    shader_setFloat(g_computeShader, "u_frequency", data->frequency);
    shader_setFloat(g_computeShader, "u_lacunarity", data->lacunarity);
    shader_setInt(g_computeShader, "u_octaves", data->octaves);
    shader_setFloat(g_computeShader, "u_scale", data->scale);
    shader_setFloat(g_computeShader, "u_amplitude", data->amplitude);
    shader_setFloat(g_computeShader, "u_offset", data->offsetXY);
    shader_setInt(g_computeShader, "u_res", texture_res[0]);

    ivec3 workGroupSize;
    shader_getWorkgroupSize(g_computeShader, workGroupSize);

    ivec3 numWorkGroups;
    glm_ivec3_div(texture_res, workGroupSize, numWorkGroups);

    glDispatchCompute(numWorkGroups[0], numWorkGroups[1], numWorkGroups[2]);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void shader_computeTerrain(InputData* data) {
    shader_useShader(g_terrainCompShader);
    shader_setFloat(g_terrainCompShader, "u_persistence", data->persistence);
    shader_setFloat(g_terrainCompShader, "u_frequency", data->frequency);
    shader_setFloat(g_terrainCompShader, "u_lacunarity", data->lacunarity);
    shader_setInt(g_terrainCompShader, "u_octaves", data->octaves);
    shader_setFloat(g_terrainCompShader, "u_scale", data->scale);
    shader_setFloat(g_terrainCompShader, "u_amplitude", data->amplitude);
    shader_setFloat(g_terrainCompShader, "u_offset", data->offsetXY);
    shader_setInt(g_terrainCompShader, "u_res", terrain_res[0]);

    glDispatchCompute((CHUNK_XZ + 7) / 8, (CHUNK_Y + 7) / 8, (CHUNK_XZ + 7) / 8);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    data->reloadTerrain = false;
}

uint8_t* shader_getTerrainData(int size_x, int size_y, int size_z) {
    const GLsizei chunkSize = size_x * size_y * size_z;
    uint8_t* data = malloc(chunkSize);
    glBindTextureUnit(TERRAIN_TEX_UNIT, g_terrainTexture);
    glGetTextureImage(g_terrainTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, chunkSize, data);
    return data;
}

void shader_cleanup(void) {
    g_activeShader = NULL;

#define DELETE_SHADER(sh) if (sh) { shader_deleteShader(&sh); sh = NULL; }

    DELETE_SHADER(g_noiseShader);
    DELETE_SHADER(g_quadShader);
    DELETE_SHADER(g_terrainShader);
    DELETE_SHADER(g_computeShader);
    DELETE_SHADER(g_terrainCompShader);

#undef DELETE_SHADER

    texture_deleteTexture(&g_computeTexture);
    texture_deleteTexture(&g_terrainTexture);

    for (int i = 0; i < TEX_COUNT; i++) {
        texture_deleteTexture(&g_voxelTexIDs[i]);
    }
}
