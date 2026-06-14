#include "scene.h"
#include "input.h"
#include "macros.h"
#include "mesh.h"
#include "shader.h"

static Mesh* g_plane = NULL;
static Mesh* g_quad = NULL;
static ChunkMesh *g_terrain = NULL;
static Camera* g_freeCam = NULL;

void scene_updatePlaneRes(int newRes) {
    if (g_plane) {
        mesh_disposeMesh(&g_plane);
        g_plane = NULL;
    }
    g_plane = mesh_createPlane(newRes);
}

static void drawPlane(InputData *data) {
    shader_activateNoiseShader();

    scene_pushMatrix();
    {
        mat4 mvp;
        scene_getMVP(mvp);
        shader_setMVP(mvp);

        shader_setNoisePlaneData(data);

        mesh_drawMesh(g_plane);
    }
    scene_popMatrix();
}

static void drawTerrain(void) {
    shader_activateTerrainShader();

    scene_pushMatrix();
    {
        mat4 mvp;
        scene_getMVP(mvp);

        scene_scale(0.1f, 0.1f, 0.1f);

        shader_setMVP(mvp);

        mesh_drawChunkMesh(g_terrain);
    }
    scene_popMatrix();
}

void scene_init(ProgContext ctx) {
    g_plane = mesh_createPlane(INITIAL_PLANE_RES);

    Vertex quadVerts[] = {
        Vertex3Tex(0, 0, 0, 0, 0, 1, 0, 0),
        Vertex3Tex(1, 0, 0, 0, 0, 1, 1, 0),
        Vertex3Tex(0, 1, 0, 0, 0, 1, 0, 1),
        Vertex3Tex(1, 1, 0, 0, 0, 1, 1, 1)
    };

    GLuint quadIndices[] = {
        0, 1, 2,
        1, 3, 2
    };

    g_quad = mesh_createMesh(
        "Quad",
        quadVerts, ARRAYSIZE(quadVerts),
        quadIndices, ARRAYSIZE(quadIndices),
        GL_TRIANGLES
    );

    g_freeCam = camera_createCamera(ctx, (vec3){0, 10, 10}, 5.0f, 15.0f, 0.1f, -90, -50);

    int w, h;
    window_getFramebufferSize(ctx, &w, &h);
    callbackFramebuffer(ctx, w, h);
}

Camera* scene_getCamera(void) {
    return g_freeCam;
}

void scene_draw(InputData* data) {
    glPolygonMode(GL_FRONT_AND_BACK, data->wireframe ? GL_LINE : GL_FILL);

    scene_pushMatrix();
    {
        shader_dispatchCompute(data);

        if (data->reloadTerrain) {
            shader_computeTerrain(data);
            if (g_terrain) {
                mesh_deleteChunkMesh(g_terrain);
            }

            uint8_t *terrainData = shader_getTerrainData(CHUNK_XZ, CHUNK_Y, CHUNK_XZ);
            g_terrain = mesh_createChunkMesh(terrainData, CHUNK_XZ, CHUNK_Y, CHUNK_XZ);
            free(terrainData);
        }

        camera_setMatrix(g_freeCam);

        // activate OpenGL states right before rendering
        // to ensure nothing that happens before changes them
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (data->renderTerrain) {
            drawTerrain();
        } else {
            drawPlane(data);
        }
    }
    scene_popMatrix();
}

void scene_drawNoiseQuad(void) {
    shader_activateQuadShader();
    shader_setNoiseGuiData();

    mat4 mvp;
    glm_mat4_identity(mvp);
    mvp[0][0] = 2;
    mvp[1][1] = 2;
    mvp[3][0] = -1;
    mvp[3][1] = -1;
    shader_setMVP(mvp);

    mesh_drawMesh(g_quad);
}

void scene_cleanup(void) {
    camera_deleteCamera(&g_freeCam);
    g_freeCam = NULL;

    if (g_plane) {
        mesh_disposeMesh(&g_plane);
        g_plane = NULL;
    }

    if (g_quad) {
        mesh_disposeMesh(&g_quad);
        g_quad = NULL;
    }

    if (g_terrain) {
        mesh_deleteChunkMesh(g_terrain);
        g_terrain = NULL;
    }
}
