#include "Utils/QuadTree.hpp"

namespace sd {

Collidable::Collidable() : qt(nullptr) {}

Collidable::Collidable(const Rect &rect, std::any data)
    : bound(rect), data(data), qt(nullptr) {}

QuadTree::QuadTree()
    : m_isLeaf(true),
      m_level(0),
      m_capacity(0),
      m_maxLevel(0),
      m_parent(nullptr),
      m_children{nullptr, nullptr, nullptr, nullptr} {}

QuadTree::QuadTree(const Rect &bound, uint32_t capacity, uint32_t maxLevel)
    : m_isLeaf(true),
      m_bound(bound),
      m_level(0),
      m_capacity(capacity),
      m_maxLevel(maxLevel),
      m_parent(nullptr),
      m_children{nullptr, nullptr, nullptr, nullptr} {
    m_objects.reserve(m_capacity);
    m_foundObjects.reserve(m_capacity);
}

void QuadTree::subdivide() {
    float width = m_bound.width * 0.5f;
    float height = m_bound.height * 0.5f;
    float x = 0, y = 0;
    for (unsigned i = 0; i < 4; ++i) {
        switch (i) {
            case 0:
                x = m_bound.x + width;
                y = m_bound.y;
                break;  // Top right
            case 1:
                x = m_bound.x;
                y = m_bound.y;
                break;  // Top left
            case 2:
                x = m_bound.x;
                y = m_bound.y + height;
                break;  // Bottom left
            case 3:
                x = m_bound.x + width;
                y = m_bound.y + height;
                break;  // Bottom right
        }
        m_children[i] =
            new QuadTree({x, y, width, height}, m_capacity, m_maxLevel);
        m_children[i]->m_level = m_level + 1;
        m_children[i]->m_parent = this;
    }
    m_isLeaf = false;
}

}  // namespace sd
