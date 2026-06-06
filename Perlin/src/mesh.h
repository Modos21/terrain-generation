#pragma once

#include <fhwcg/fhwcg.h>

typedef enum {
    M_RECTANGLE,
    M_CIRCLE,
    M_TYPE_COUNT
} CGMeshType;

typedef struct {
    vec2 pos;
    vec2 texCoord;
} CGVertex;

#define CG_VERT(x, y, u, v) (CGVertex){.pos = {x, y}, .texCoord = {u, v}}

typedef struct {
    GLuint vao;
    GLuint vbo;
    uint vertexCount;
    uint vertexSize;
    GLenum primitive;
} CGMesh;

CGMesh *mesh_create(CGVertex* vertices, uint vertexCount, int vertexSize, GLenum primitive);

CGMesh *mesh_createQuad(void);

CGMesh *mesh_createCircle(uint detail);

void mesh_draw(CGMesh *mesh);

void mesh_deleteMesh(CGMesh *mesh);
