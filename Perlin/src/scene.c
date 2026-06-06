#include "scene.h"
#include "input.h"
#include "macros.h"
#include "shader.h"

#define TERRAIN_SIZE_XZ 10

static Mesh* g_plane = NULL;
static Mesh* g_quad = NULL;
static Camera* g_freeCam = NULL;

static void createPlane(const int res) {
    const int numVertices = (res + 1) * (res + 1);
    const int numIndices = res * res * 6;

    Vertex* vertices = malloc(sizeof(Vertex) * numVertices);
    GLuint* indices = malloc(sizeof(GLuint) * numIndices);

    // Vertex-Erzeugung
    for (int z = 0; z <= res; ++z) {
        for (int x = 0; x <= res; ++x) {
            float fx = ((float)x / (float)res - 0.5f) * TERRAIN_SIZE_XZ;
            float fz = ((float)z / (float)res - 0.5f) * TERRAIN_SIZE_XZ;

            int index = z * (res + 1) + x;
            vertices[index] = Vertex3Tex(
                fx, 0.0f, fz,
                0.0f, 1.0f, 0.0f,
                (float) x / res,
                (float) z / res
            );
        }
    }

    // Index-Erzeugung (2 Dreiecke pro Quad)
    int i = 0;
    for (int z = 0; z < res; ++z) {
        for (int x = 0; x < res; ++x) {
            int topLeft = z * (res + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (res + 1);
            int bottomRight = bottomLeft + 1;

            // Dreieck 1: CCW
            indices[i++] = topLeft;
            indices[i++] = bottomLeft;
            indices[i++] = topRight;

            // Dreieck 2: CCW (bottomRight und bottomLeft geswappt für korrekte Winding Order)
            indices[i++] = topRight;
            indices[i++] = bottomRight;
            indices[i++] = bottomLeft;
        }
    }

    g_plane = mesh_createMesh("Plane", vertices, numVertices, indices, numIndices, GL_TRIANGLES);

    free(vertices);
    free(indices);
}

void scene_updatePlaneRes(int newRes) {
    if (g_plane) {
        mesh_disposeMesh(&g_plane);
        g_plane = NULL;
    }
    createPlane(newRes);
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

void scene_init(ProgContext ctx) {
    createPlane(INITIAL_PLANE_RES);

    Vertex quadVerts[] = {
        Vertex3Tex(0, 0, 0, 0, 0, 1, 0, 0),
        Vertex3Tex(1, 0, 0, 0, 0, 1, 1, 0),
        Vertex3Tex(0, 1, 0, 0, 0, 1, 0, 1),
        Vertex3Tex(1, 1, 0, 0, 0, 1, 1, 1)
    };

    GLuint quadIndices[] = {
        0, 1, 2,
        1, 2, 3
    };

    g_quad = mesh_createMesh(
        "Quad",
        quadVerts, ARRAYSIZE(quadVerts),
        quadIndices, ARRAYSIZE(quadIndices),
        GL_TRIANGLES
    );

    g_freeCam = camera_createCamera(ctx, (vec3){0, 10, 10}, 2.0f, 5.0f, 0.1f, -90, -50);

    int w, h;
    window_getFramebufferSize(ctx, &w, &h);
    callbackFramebuffer(ctx, w, h);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

Camera* scene_getCamera(void) {
    return g_freeCam;
}

void scene_draw(InputData* data) {
    glPolygonMode(GL_FRONT_AND_BACK, data->wireframe ? GL_LINE : GL_FILL);

    scene_pushMatrix();
    {
        shader_dispatchCompute(data);

        camera_setMatrix(g_freeCam);

        drawPlane(data);
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
}
