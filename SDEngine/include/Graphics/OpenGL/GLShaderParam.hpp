#ifndef SD_GL_SHADER_PARAM_HPP
#define SD_GL_SHADER_PARAM_HPP

#include "Graphics/ShaderParam.hpp"

namespace SD {

class GLShaderParam : public ShaderParam {
   public:
    GLShaderParam(UniformType type, const std::string& name, int32_t index,
                  uint32_t program_id, int32_t location, int32_t tex_binding_id,
                  int32_t image_binding, int32_t size);
    int32_t GetLocation() const { return m_location; }

    void SetAsBool(bool value) override;

    void SetAsInt(int value) override;

    void SetAsUint(uint32_t value) override;

    void SetAsFloat(float value) override;

    void SetAsVec(const int* value, int32_t count) override;

    void SetAsVec(const float* value, int32_t count) override;

    void SetAsVec2(const float* value, int32_t count) override;

    void SetAsVec3(const float* value, int32_t count) override;

    void SetAsVec4(const float* value, int32_t count) override;

    void SetAsMat3(const float* value, int32_t count) override;

    void SetAsMat4(const float* value, int32_t count) override;

    void SetAsTexture(const Texture* texture) override;
    void SetAsTextures(const Texture** texture, int count) override;
    void SetAsImage(const Texture* texture, int32_t level, bool layered,
                    int32_t layer, Access access) override;

   private:
    uint32_t m_program;
    int32_t m_location;
    int32_t m_texture_binding;
    int32_t m_image_binding;
};

}  // namespace SD
#endif
