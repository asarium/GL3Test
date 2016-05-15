#pragma once

#include <renderer/BufferObject.hpp>

#include <memory>
#include <renderer/VertexLayout.hpp>
#include <renderer/DrawCall.hpp>
#include <renderer/Renderer.hpp>


class DrawUtil {
    std::unique_ptr<BufferObject> _fullscreenTriBuffer;
    std::unique_ptr<VertexLayout> _fullscreenTriLayout;
    std::unique_ptr<DrawCall> _fullscreenTriDrawCall;

    std::unique_ptr<BufferObject> _quadBuffer;
    std::unique_ptr<VertexLayout> _quadLayout;
    std::unique_ptr<DrawCall> _quadDrawCall;

    std::unique_ptr<BufferObject> _sphereVertexData;
    std::unique_ptr<BufferObject> _sphereIndexData;
    std::unique_ptr<VertexLayout> _sphereLayout;
    std::unique_ptr<DrawCall> _sphereDrawCall;
 public:
    DrawUtil(Renderer* renderer);

    void drawSphere();

    void drawFullscreenTri();
};


