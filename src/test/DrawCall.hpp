#pragma once


#include <cstdint>
#include <renderer/CommandBuffer.hpp>

class DrawCall {
    bool _indexed;

    uint32_t _count;
    uint32_t _offset;

    uint32_t _baseVertex;
 public:
    DrawCall();

    DrawCall& indexed(uint32_t count, uint32_t offset);

    DrawCall& array(uint32_t count, uint32_t offset);

    DrawCall& baseVertex(uint32_t base);

    void draw(CommandBuffer* cmd, uint32_t instances = 1);
};


