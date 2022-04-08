#include "Graphics/ModelNode.hpp"

namespace SD {

ModelNode::ModelNode(const Matrix4f &transform) : m_local_matrix(transform) {}

ModelNode::~ModelNode()
{
    for (auto &node : m_children) {
        delete node;
    }
}
}  // namespace SD
