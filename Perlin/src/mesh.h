#pragma once

#include <fhwcg/fhwcg.h>

#define CHUNK_XZ 64
#define CHUNK_Y 64

typedef struct {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
    // voxel type
    uint8_t type;
} VoxelVertex;

typedef struct {
    GLuint vao, vbo, ebo;
    GLsizei indexCount;
} VoxelMesh;

#define VV(type, x, y, z, nx, ny, nz, u, v) (VoxelVertex){.pos = {x,y,z}, .normal = {nx,ny,nz}, .texCoord= {u,v}, .type = type}

static const int DIRS[6][3] = {
    {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}
};

static const float NORMALS[6][3] = {
    {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}
};

static const float FACE_VERTS[6][4][3] = {
    // +X
    {{1,0,0},{1,1,0},{1,1,1},{1,0,1}},
    // -X
    {{0,0,1},{0,1,1},{0,1,0},{0,0,0}},
    // +Y (oben)
    {{0,1,0},{0,1,1},{1,1,1},{1,1,0}},
    // -Y (unten)
    {{0,0,1},{0,0,0},{1,0,0},{1,0,1}},
    // +Z
    {{1,0,1},{1,1,1},{0,1,1},{0,0,1}},
    // -Z
    {{0,0,0},{0,1,0},{1,1,0},{1,0,0}},
};

// UVs für jedes der 4 Vertices (passend zur Reihenfolge oben)
static const float FACE_UVS[4][2] = {
    {0,0},{0,1},{1,1},{1,0}
};

VoxelMesh *mesh_createVoxelMesh(const uint8_t *data, int size_x, int size_y, int size_z);

void mesh_drawVoxelMesh(VoxelMesh *mesh);

void mesh_deleteVoxelMesh(VoxelMesh *mesh);