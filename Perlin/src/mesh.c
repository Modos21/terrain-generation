#include "mesh.h"

#define X 0
#define Y 1
#define Z 2
#define IN_BOUNDS(v, v_max) (v >= 0 && v < v_max)

#define AT(data, x, y, z) data[(z)*CHUNK_XZ*CHUNK_Y + (y)*CHUNK_XZ + (x)]
#define IS_FLUID(block) (block == WATER || block == LAVA)

enum BlockType {
    AIR,
    GRASS,
    DIRT,
    STONE,
    WATER,
    LAVA,
    SAND,
    BEDROCK
};

static VoxelMesh *createVoxelMesh(VoxelVertex *verts, int vertCount, GLuint *indices, int indexCount) {
    VoxelMesh *m = malloc(sizeof(VoxelMesh));

    // create vao
    glCreateVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    // create vbo
    glCreateBuffers(1, &m->vbo);
    glNamedBufferStorage(m->vbo, vertCount * (int) sizeof(VoxelVertex), verts, 0);
    glVertexArrayVertexBuffer(m->vao, 0, m->vbo, 0, sizeof(VoxelVertex));
    // create ebo
    glCreateBuffers(1, &m->ebo);
    glNamedBufferStorage(m->ebo, indexCount * (int) sizeof(GLuint), indices, 0);
    glVertexArrayElementBuffer(m->vao, m->ebo);

    // Position
    glEnableVertexArrayAttrib(m->vao, 0);
    glVertexArrayAttribFormat(m->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VoxelVertex, pos));
    glVertexArrayAttribBinding(m->vao, 0, 0);

    glEnableVertexArrayAttrib(m->vao, 1);
    glVertexArrayAttribFormat(m->vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VoxelVertex, normal));
    glVertexArrayAttribBinding(m->vao, 1, 0);


    glEnableVertexArrayAttrib(m->vao, 2);
    glVertexArrayAttribFormat(m->vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(VoxelVertex, texCoord));
    glVertexArrayAttribBinding(m->vao, 2, 0);


    glEnableVertexArrayAttrib(m->vao, 3);
    glVertexArrayAttribIFormat(m->vao, 3, 1, GL_UNSIGNED_BYTE, offsetof(VoxelVertex, type));
    glVertexArrayAttribBinding(m->vao, 3, 0);

    glBindVertexArray(0);
    m->indexCount = indexCount;

    return m;
}

ChunkMesh* mesh_createChunkMesh(const uint8_t* data, int size_x, int size_y, int size_z) {
    const uint chunkSize = size_x * size_y * size_z;

    const uint vertArrSize = chunkSize * 6 * 4 * sizeof(VoxelVertex);
    VoxelVertex* solidVerts = malloc(vertArrSize);
    GLuint* solidIdxs = malloc(chunkSize * 6 * 6 * sizeof(GLuint));
    int solidVC = 0, solidIC = 0;

    VoxelVertex* waterVerts = malloc(vertArrSize);
    GLuint *waterIdxs = malloc(chunkSize * 6 * 6 * sizeof(GLuint));
    int waterVC = 0, waterIC = 0;

    // loop over all voxels in chunk
    for (int z = 0; z < size_z; z++) {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                uint8_t block = AT(data, x, y, z);

                if (block == AIR) continue;

                // choose buffer for block
                VoxelVertex *vbuf = (block == WATER) ? waterVerts : solidVerts;
                GLuint *ibuf = (block == WATER) ? waterIdxs : solidIdxs;
                int *vc = (block == WATER) ? &waterVC : &solidVC;
                int *ic = (block == WATER) ? &waterIC : &solidIC;

                bool fluidAbove = IS_FLUID(AT(data, x, y+1, z));

                for (int d = 0; d < 6; d++) {
                    int nx = x + DIRS[d][X];
                    int ny = y + DIRS[d][Y];
                    int nz = z + DIRS[d][Z];

                    int neighbor = AIR;
                    if (IN_BOUNDS(nx, size_x) && IN_BOUNDS(ny, size_y) && IN_BOUNDS(nz, size_z)) {
                        neighbor = AT(data, nx, ny, nz);
                    }

                    // only skip solids if there's no fluid next to them to also show the side of the blocks
                    // because fluids are slightly lower and would expose the skipped sides
                    if (!IS_FLUID(block) && neighbor != AIR && !IS_FLUID(neighbor)) continue;

                    // only skip fluid faces if there's a fluid above or anything other than air to the sides
                    if (IS_FLUID(block) && (d == 2 && IS_FLUID(neighbor) || d != 2 && neighbor != AIR)) continue;

                    float blockY = 1.0f;
                    // if the block is a fluid and there is no fluid above it, make it slightly smaller along Y
                    if (IS_FLUID(block) && fluidAbove) {
                       blockY = 0.9f;
                    }

                    for (int v = 0; v < 4; v++) {
                        vbuf[*vc + v] = (VoxelVertex){
                            .pos = {
                                (float)x + FACE_VERTS[d][v][X],
                                (float)y + FACE_VERTS[d][v][Y] * blockY,
                                (float)z + FACE_VERTS[d][v][Z],
                            },
                            .normal = {
                                NORMALS[d][0],
                                NORMALS[d][1],
                                NORMALS[d][2],
                            },
                            .texCoord = {
                                FACE_UVS[v][0],
                                FACE_UVS[v][1],
                            },
                            .type = block
                        };
                    }
                    // 2 triangles (6 indices) from 4 verts
                    uint32_t b = *vc;
                    ibuf[(*ic)++] = b + 0;
                    ibuf[(*ic)++] = b + 1;
                    ibuf[(*ic)++] = b + 2;

                    ibuf[(*ic)++] = b + 0;
                    ibuf[(*ic)++] = b + 2;
                    ibuf[(*ic)++] = b + 3;
                    *vc += 4;
                }
            }
        }
    }

    ChunkMesh *mesh = malloc(sizeof(ChunkMesh));
    mesh->solidTerrain = createVoxelMesh(solidVerts, solidVC, solidIdxs, solidIC);
    mesh->waterTerrain = createVoxelMesh(waterVerts, waterVC, waterIdxs, waterIC);

    free(solidVerts); free(solidIdxs);
    free(waterVerts); free(waterIdxs);

    return mesh;
}

void mesh_drawVoxelMesh(VoxelMesh* mesh) {
    if (mesh->indexCount <= 0) return;

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void mesh_drawChunkMesh(ChunkMesh* mesh) {
    // draw solid mesh first with depth test, culling, and no blending
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    mesh_drawVoxelMesh(mesh->solidTerrain);

    // draw water mesh with blending and no culling
    glEnable(GL_BLEND);
    // because the water is drawn front to back, we have to swap the factors
    // and also use 1-alpha for our materials in the shader
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    mesh_drawVoxelMesh(mesh->waterTerrain);

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

void mesh_deleteVoxelMesh(VoxelMesh* mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    free(mesh);
    mesh = NULL;
}

void mesh_deleteChunkMesh(ChunkMesh* mesh) {
    mesh_deleteVoxelMesh(mesh->solidTerrain);
    mesh_deleteVoxelMesh(mesh->waterTerrain);
    free(mesh);
    mesh = NULL;
}




