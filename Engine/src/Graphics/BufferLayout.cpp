#include "Graphics/BufferLayout.hpp"

namespace SD {

uint32_t GetBufferDataSize(BufferDataType type) {
    switch (type) {
        case BufferDataType::UCHAR:
            return 1;
        case BufferDataType::UINT:
        case BufferDataType::FLOAT:
            return 4;
        case BufferDataType::FLOAT2:
            return 2 * 4;
        case BufferDataType::FLOAT3:
            return 3 * 4;
        case BufferDataType::FLOAT4:
            return 4 * 4;
    }
    return 0;
}
uint32_t GetComponentCount(BufferDataType type) {
    switch (type) {
        case BufferDataType::UCHAR:
        case BufferDataType::UINT:
        case BufferDataType::FLOAT:
            return 1;
        case BufferDataType::FLOAT2:
            return 2;
        case BufferDataType::FLOAT3:
            return 3;
        case BufferDataType::FLOAT4:
            return 4;
    }
    return 0;
}

VertexBufferLayout::VertexBufferLayout(uint32_t instance_stride)
    : m_stride(0), m_instance_stride(instance_stride) {}

void VertexBufferLayout::Push(BufferDataType type, bool normalized) {
    m_elements.push_back({type, GetComponentCount(type), normalized});
    m_stride += GetBufferDataSize(type);
}

void VertexBufferLayout::Clear() {
    m_elements.clear();
    m_elements.shrink_to_fit();
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
