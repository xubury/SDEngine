#include "Graphics/VertexArray.hpp"
#include "Renderer/Renderer.hpp"
#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<VertexArray> VertexArray::create() {
    Ref<VertexArray> va;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            va = createRef<GLVertexArray>();
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (va) va->init();
    return va;
}

void VertexArray::setIndexBuffer(const Ref<IndexBuffer> &buffer) {
    bind();
    buffer->bind();
    m_indexBuffer = buffer;
}

const Ref<IndexBuffer> VertexArray::getIndexBuffer() const { return m_indexBuffer; }

}  // namespace sd
