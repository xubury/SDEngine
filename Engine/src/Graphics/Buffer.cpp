#include "Graphics/Buffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Core/Log.hpp"

namespace sd {

Ref<VertexBuffer> VertexBuffer::create(const void *data, size_t size,
                                       BufferIOType io) {
    Ref<VertexBuffer> vb;
    switch (Device::getAPI()) {
        case API::OpenGL:
            vb = createRef<GLVertexBuffer>(data, size, TRANSLATE(io));
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return vb;
}

Ref<IndexBuffer> IndexBuffer::create(const uint32_t *data, uint32_t count,
                                     BufferIOType io) {
    Ref<IndexBuffer> eb;
    switch (Device::getAPI()) {
        case API::OpenGL:
            eb = createRef<GLIndexBuffer>(data, count, TRANSLATE(io));
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return eb;
}

Ref<UniformBuffer> UniformBuffer::create(const void *data, size_t size,
                                         BufferIOType io) {
    Ref<UniformBuffer> ub;
    switch (Device::getAPI()) {
        case API::OpenGL:
            ub = createRef<GLUniformBuffer>(data, size, TRANSLATE(io));
            break;
        case API::None:
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return ub;
}

}  // namespace sd
