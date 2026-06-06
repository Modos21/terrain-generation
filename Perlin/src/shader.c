#include "shader.h"

static Shader* g_noiseShader = NULL;
static Shader* g_quadShader = NULL;
static Shader* g_computeShader = NULL;

static Shader* g_activeShader = NULL;

static GLuint g_computeTexture = 0;
static ivec3 texture_res = {512, 512, 1};

static GLuint createTexture(GLint internalFormat, const ivec2 size) {
    GLuint texId;
    glCreateTextures(GL_TEXTURE_2D, 1, &texId);

    glTextureStorage2D(texId, 1, internalFormat, size[0], size[1]);

    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texId;
}

void shader_init(void) {
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

    g_computeShader = shader_createShader();
    shader_attachShaderFile(g_computeShader, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/fbm.comp");

    if (!shader_buildShader("Compute", &g_computeShader)) {
        exit(EXIT_FAILURE);
    }

    g_computeTexture = createTexture(GL_RGBA32F, texture_res);
    glBindTextureUnit(NOISE_TEX_UNIT, g_computeTexture);
    glBindImageTexture(NOISE_TEX_UNIT, g_computeTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void shader_reload(void) {
    Shader *newNoise = shader_createVeFrShader("Noise",
            RESOURCE_PATH "shader/noise.vert", RESOURCE_PATH "shader/noise.frag");

    if (newNoise) {
        shader_deleteShader(&g_noiseShader);
        g_noiseShader = newNoise;
    }

    Shader *newQuad = shader_createVeFrShader("Noise Quad",
            RESOURCE_PATH "shader/quad.vert", RESOURCE_PATH "shader/quad.frag");

    if (newQuad) {
        shader_deleteShader(&g_quadShader);
        g_quadShader = newQuad;
    }

    Shader *newComp = shader_createShader();
    shader_attachShaderFile(newComp, GL_COMPUTE_SHADER, RESOURCE_PATH "shader/fbm.comp");

    if (shader_buildShader("Compute", &newComp) && newComp) {
        shader_deleteShader(&g_computeShader);
        g_computeShader = newComp;
    }
}

void shader_activateNoiseShader(void) {
    shader_useShader(g_noiseShader);
    g_activeShader = g_noiseShader;
}

void shader_activateQuadShader(void) {
    shader_useShader(g_quadShader);
    g_activeShader = g_quadShader;
}

void shader_setMVP(mat4 mvp) {
    shader_setMat4(g_activeShader, "u_mvpMatrix", (mat4*)mvp);
}

void shader_setNoisePlaneData(InputData *data) {
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

void shader_cleanup(void) {
    g_activeShader = NULL;

#define DELETE_SHADER(sh) if (sh) { shader_deleteShader(&sh); sh = NULL; }

    DELETE_SHADER(g_noiseShader);
    DELETE_SHADER(g_quadShader);
    DELETE_SHADER(g_computeShader);

#undef DELETE_SHADER

    texture_deleteTexture(&g_computeTexture);
}
