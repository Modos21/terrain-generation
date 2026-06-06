#include "mesh.h"

CGMesh* mesh_create(CGVertex* vertices, uint vertexCount, int vertexSize, GLenum primitive) {
    uint dataSize = vertexCount * vertexSize;
    CGMesh *mesh = malloc(dataSize);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, dataSize, vertices, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, vertexSize);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(CGVertex, pos));
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(CGVertex, texCoord));
    glVertexArrayAttribBinding(vao, 1, 0);

    mesh->vertexSize = vertexSize;
    mesh->vertexCount = vertexCount;
    mesh->vao = vao;
    mesh->vbo = vbo;
    mesh->primitive = primitive;

    return mesh;
}

CGMesh* mesh_createQuad(void) {
    CGVertex quadVerts[] = {
        CG_VERT(-0.5f, -0.5f, 0, 0),
        CG_VERT( 0.5f, -0.5f, 1, 0),
        CG_VERT( 0.5f,  0.5f, 1, 1),
        CG_VERT(-0.5f, -0.5f, 0, 0),
        CG_VERT( 0.5f,  0.5f, 1, 1),
        CG_VERT(-0.5f,  0.5f, 0, 1)
    };
    return mesh_create(quadVerts, ARRAYSIZE(quadVerts), sizeof(quadVerts[0]), GL_TRIANGLES);
}

CGMesh* mesh_createCircle(uint detail) {
    CGVertex *ballVerts = malloc(sizeof(CGVertex) * (detail + 2));
    ballVerts[0] = CG_VERT(0, 0, 0, 0);

    float angleStep = 2 * GLM_PIf / detail;
    for (uint i = 0; i <= detail; i++) {
        float x = cosf(angleStep * (float) i);
        float y = sinf(angleStep * (float) i);
        ballVerts[1 + i] = CG_VERT(x, y, 0, 0);
    }
    CGMesh *res = mesh_create(ballVerts, detail + 2, sizeof(ballVerts[0]), GL_TRIANGLE_FAN);
    free(ballVerts);
    return res;
}

void mesh_draw(CGMesh* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawArrays(mesh->primitive, 0, (int) mesh->vertexCount);
    glBindVertexArray(0);
}

void mesh_deleteMesh(CGMesh* mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    free(mesh);
    mesh = NULL;
}
