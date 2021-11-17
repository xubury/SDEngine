#include "Graphics/BufferLayout.hpp"

namespace SD {

uint32_t GetSizeOfType(BufferDataType type) {
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

VertexBufferLayout::VertexBufferLayout(uint32_t instanceStride)
    : m_stride(0), m_instanceStride(instanceStride) {}

void VertexBufferLayout::Push(BufferDataType type, uint32_t count,
                              bool normalized) {
    m_elements.push_back({type, count, normalized});
    m_stride += count * GetSizeOfType(type);
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
    return m_instanceStride;
}

}  // namespace SD
