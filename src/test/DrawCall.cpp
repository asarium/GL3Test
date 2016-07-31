//
//

#include <util/Assertion.hpp>
#include "DrawCall.hpp"

DrawCall::DrawCall() : _indexed(false), _count(0), _offset(0), _baseVertex(0) {
}
DrawCall& DrawCall::indexed(uint32_t count, uint32_t offset) {
    _indexed = true;
    _count = count;
    _offset = offset;

    return *this;
}
DrawCall& DrawCall::array(uint32_t count, uint32_t offset) {
    _indexed = false;
    _count = count;
    _offset = offset;

    return *this;
}
DrawCall& DrawCall::baseVertex(uint32_t base) {
    Assertion(_indexed, "Base vertex is only effective on indexed draw calls!");

    _baseVertex = base;

    return *this;
}
void DrawCall::draw(CommandBuffer* cmd, uint32_t instances) {
    if (_indexed) {
        cmd->drawIndexed(_count, instances, _offset, _baseVertex, 0);
    } else {
        cmd->draw(_count, instances, _offset, 0);
    }
}
