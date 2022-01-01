#include "Graphics/BufferLayout.hpp"

namespace SD {

uint32_t GetBufferDataSize(BufferLayoutType type) {
    switch (type) {
        case BufferLayoutType::UBYTE:
            return 1;
        case BufferLayoutType::UINT:
        case BufferLayoutType::INT:
        case BufferLayoutType::FLOAT:
            return 4;
        case BufferLayoutType::FLOAT2:
            return 2 * 4;
        case BufferLayoutType::FLOAT3:
            return 3 * 4;
        case BufferLayoutType::FLOAT4:
            return 4 * 4;
    }
    return 0;
}
uint32_t GetComponentCount(BufferLayoutType type) {
    switch (type) {
        case BufferLayoutType::UBYTE:
        case BufferLayoutType::UINT:
        case BufferLayoutType::INT:
        case BufferLayoutType::FLOAT:
            return 1;
        case BufferLayoutType::FLOAT2:
            return 2;
        case BufferLayoutType::FLOAT3:
            return 3;
        case BufferLayoutType::FLOAT4:
            return 4;
    }
    return 0;
}

VertexBufferLayout::VertexBufferLayout(uint32_t instance_stride)
    : m_stride(0), m_instance_stride(instance_stride) {}

void VertexBufferLayout::Push(BufferLayoutType type, bool normalized) {
    m_elements.push_back({type, GetComponentCount(type), normalized});
    m_stride += GetBufferDataSize(type);
}

void VertexBufferLayout::Clear() {
    m_elements.clear();
    m_elements.shrink_to_fit();
    m_stride = 0;
}

const std::vector<VertexBufferLayoutElement> &VertexBufferLayout::GetElements()
    const {
    return m_elements;
}

uint32_t VertexBufferLayout::GetStride() const { return m_stride; }

uint32_t VertexBufferLayout::GetInstanceStride() const {
    return m_instance_stride;
}

}  // namespace SD
