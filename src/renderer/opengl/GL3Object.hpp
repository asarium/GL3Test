#pragma once

class GL3Renderer;

class GL3Object {
protected:
    GL3Renderer* _renderer;

    GL3Object(GL3Renderer* renderer);
};


