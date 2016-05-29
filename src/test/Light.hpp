#pragma once

#include <renderer/Renderer.hpp>

#include <glm/glm.hpp>

namespace lighting
{
    struct LightParameters
    {
        glm::mat4 light_view_proj_matrix;
        glm::mat4 model_matrix;

        glm::vec3 light_vector;
        int32_t light_type;
        glm::vec3 light_color;
        uint32_t light_has_shadow;

        glm::vec2 frag_coord_scale;
    };

    struct ShadowMatrices {
        glm::mat4 projection;
        glm::mat4 view;
    };

    enum class LightType {
        Directional,
        Point,
        Ambient
    };

    class LightingManager;

    class Light {
        Renderer* _renderer;
        LightingManager* _manager;

        LightType _type;
        glm::vec3 _position;
        glm::vec3 _direction;
        glm::vec3 _color;

        bool _shadowing;
        std::unique_ptr<PipelineState> _shadowPassPipelinestate;
        std::unique_ptr<DescriptorSet> _lightDescriptorSet;
        Descriptor* _uniformDescriptor;

        std::unique_ptr<RenderTarget> _shadowMapTarget;

        ShadowMatrices _matricies;
    public:
        Light(Renderer* renderer, LightingManager* manager, LightType type, bool shadowing);
        ~Light() { }

        void setPosition(const glm::vec3& pos);

        void setDirection(const glm::vec3& dir);

        void setColor(const glm::vec3& color);

        void setParameters(LightParameters* params);

        LightType getType();

        void updateDescriptor(BufferObject* uniforms, size_t offset, size_t size);

        void bindDescriptorSet();
        void unbindDescriptorSet();

        ShadowMatrices beginShadowPass(const ViewUniformData& viewdata);

        void endShadowPass();
    };

}
