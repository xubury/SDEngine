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

int Viewport::window_height = 0;
int Viewport::window_width = 0;

int Viewport::GetLeft() const { return m_left; }

int Viewport::GetBottom() const { return window_height - m_height - GetTop(); }

int Viewport::GetTop() const { return m_top; }

void Viewport::SetSize(int x, int y, int width, int height) {
    m_width = width;
    m_height = height;
    m_left = x;
    if (GetGraphicsAPI() == GraphicsAPI::OpenGL) {
        m_top = window_height - height - y;
    } else {
        m_top = y;
    }
}

void Viewport::SetRect(int left, int top, int right, int bottom) {
    m_left = left;
    m_top = top;
    m_width = right - left;
    m_height = bottom - top;
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
