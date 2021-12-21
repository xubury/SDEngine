#include "Graphics/Graphics.hpp"

namespace SD {

static GraphicsAPI s_api = GraphicsAPI::None;

void SetGraphicsAPI(GraphicsAPI api) {
    if (s_api != GraphicsAPI::None) {
        SD_CORE_ERROR("Graphics API already set!");
        exit(-1);
    }
    switch (api) {
        case GraphicsAPI::OpenGL:
            SD_CORE_INFO("Using OpenGL as backend..");
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            exit(-1);
    }
    s_api = api;
}

GraphicsAPI GetGraphicsAPI() { return s_api; }

const std::string GetFaceName(CubeMapFace face) {
    static const std::unordered_map<CubeMapFace, std::string> name = {
        {CubeMapFace::RIGHT, "Right"}, {CubeMapFace::LEFT, "Left"},
        {CubeMapFace::TOP, "Top"},     {CubeMapFace::BOTTOM, "Bottom"},
        {CubeMapFace::FRONT, "Front"}, {CubeMapFace::BACK, "Back"}};
    return name.at(face);
}

Viewport::Viewport(int left, int top, int width, int height)
    : m_left(left), m_top(top), m_width(width), m_height(height) {
}

int Viewport::GetLeft() const { return m_left; }

int Viewport::GetTop() const { return m_top; }

int Viewport::GetWidth() const { return m_width; }

int Viewport::GetHeight() const { return m_height; }

void Viewport::SetSize(int left, int top, int width, int height) {
    m_left = left;
    m_top = top;
    m_width = width;
    m_height = height;
}

glm::vec2 Viewport::MapScreenToClip(const glm::ivec2 &screen) const {
    glm::vec2 clip(0);
    clip.x = 2.f * (screen.x - GetLeft()) / GetWidth() - 1.f;
    clip.y = 1.f - 2.f * (screen.y - GetTop()) / GetHeight();
    return clip;
}

glm::ivec2 Viewport::MapClipToScreen(const glm::vec2 &clip) const {
    glm::vec2 screen(0);
    screen.x = (clip.x + 1.f) * GetWidth() / 2.f + GetLeft();
    screen.y = (1.f - clip.y) * GetHeight() / 2.f + GetTop();
    return screen;
}

}  // namespace SD
