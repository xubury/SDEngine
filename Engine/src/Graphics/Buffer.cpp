#include "Graphics/Buffer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Utility/Log.hpp"

namespace sd {

Ref<VertexBuffer> VertexBuffer::create(const void *data, uint32_t count,
                                       uint8_t elementSize, BufferIOType io) {
    Ref<VertexBuffer> vb;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            vb = createRef<GLVertexBuffer>(data, count, elementSize, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return vb;
}

Ref<IndexBuffer> IndexBuffer::create(const void *data, uint32_t count,
                                     uint8_t elementSize, BufferIOType io) {
    Ref<IndexBuffer> eb;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            eb = createRef<GLIndexBuffer>(data, count, elementSize, io);
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
    switch (Graphics::getAPI()) {
        case API::OpenGL:
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

}  // namespace sd
