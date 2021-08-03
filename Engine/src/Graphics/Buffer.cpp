#include "Graphics/Buffer.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<VertexBuffer> VertexBuffer::create(const void *data, size_t size,
                                       BufferIOType io) {
    Ref<VertexBuffer> vb;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            vb = createRef<GLVertexBuffer>(data, size, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (vb) vb->init();
    return vb;
}

Ref<IndexBuffer> IndexBuffer::create(const uint32_t *data, uint32_t count,
                                     BufferIOType io) {
    Ref<IndexBuffer> eb;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            eb = createRef<GLIndexBuffer>(data, count, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (eb) eb->init();
    return eb;
}

Ref<UniformBuffer> UniformBuffer::create(const void *data, size_t size,
                                         BufferIOType io) {
    Ref<UniformBuffer> ub;
    switch (Renderer::getAPI()) {
        case API::OpenGL:
            ub = createRef<GLUniformBuffer>(data, size, io);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    if (ub) ub->init();
    return ub;
}

}  // namespace sd
