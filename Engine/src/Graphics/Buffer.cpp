#include "Graphics/Buffer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Utility/Log.hpp"

namespace SD {

Ref<VertexBuffer> VertexBuffer::create(const void *data, size_t size,
                                       BufferIOType io) {
    Ref<VertexBuffer> vb;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            vb = createRef<GLVertexBuffer>(data, size, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return vb;
}

Ref<IndexBuffer> IndexBuffer::create(const uint32_t *data, uint32_t count,
                                     BufferIOType io) {
    Ref<IndexBuffer> eb;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            eb = createRef<GLIndexBuffer>(data, count, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return eb;
}

Ref<UniformBuffer> UniformBuffer::create(const void *data, size_t size,
                                         BufferIOType io) {
    Ref<UniformBuffer> ub;
    switch (getGraphicsAPI()) {
        case GraphicsAPI::OpenGL:
            ub = createRef<GLUniformBuffer>(data, size, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return ub;
}

IndexBuffer::IndexBuffer(uint32_t count) : m_count(count) {}

uint32_t IndexBuffer::getCount() const { return m_count; }

}  // namespace SD
