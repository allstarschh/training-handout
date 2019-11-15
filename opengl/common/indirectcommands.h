#ifndef INDIRECTCOMMANDS_H
#define INDIRECTCOMMANDS_H

#include <qopengl.h>

struct IndirectDrawArraysCommand
{
    GLuint vertexCount;
    GLuint instanceCount;
    GLuint firstVertex;
    GLuint baseInstance;
};

struct IndirectDrawElementsCommand
{
    GLuint indexCount;
    GLuint instanceCount;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint baseInstance;
};

#endif // INDIRECTCOMMANDS_H
