#pragma once

#include "GL3VertexLayout.hpp"
#include "GL3Object.hpp"

class GL3Util : public GL3Object {
    std::unique_ptr<BufferObject> _fullscreenTriBuffer;
    std::unique_ptr<GL3VertexLayout> _fullscreenTriLayout;

    std::unique_ptr<BufferObject> _quadBuffer;
    std::unique_ptr<GL3VertexLayout> _quadLayout;

    std::unique_ptr<BufferObject> _sphereVertexData;
    GLsizei _numSphereIndices;
    std::unique_ptr<BufferObject> _sphereIndexData;
    std::unique_ptr<GL3VertexLayout> _sphereLayout;
public:
    GL3Util(GL3Renderer *renderer);

    void drawFullScreenTri();

    void drawQuad();

    void drawSphere();
};

bool checkFrameBufferStatus();


