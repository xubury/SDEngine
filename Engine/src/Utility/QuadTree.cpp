#include "Utility/QuadTree.hpp"
#include <algorithm>

namespace SD {

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

QuadTree::~QuadTree() {
    clear();
    for (int i = 0; i < 4; ++i) {
        if (m_children[i]) {
            delete m_children[i];
        }
    }
}

bool QuadTree::insert(Collidable *obj) {
    if (obj->qt != nullptr) return false;

    if (!m_isLeaf) {
        QuadTree *child = getChild(obj->bound);
        if (child) {
            return child->insert(obj);
        }
    }
    m_objects.push_back(obj);
    obj->qt = this;

    if (m_isLeaf && m_level < m_maxLevel && m_objects.size() >= m_capacity) {
        subdivide();
        update(obj);
    }
    return true;
}

bool QuadTree::remove(Collidable *obj) {
    if (obj->qt == nullptr) return false;
    if (obj->qt != this) return obj->qt->remove(obj);

    auto iter = std::find(m_objects.begin(), m_objects.end(), obj);
    if (iter != m_objects.end()) {
        m_objects.erase(iter);
        obj->qt = nullptr;
        discardEmptyBuckets();
        return true;
    }
    return false;
}

bool QuadTree::update(Collidable *obj) {
    if (!remove(obj)) return false;

    if (m_parent != nullptr && !m_bound.contains(obj->bound)) {
        return m_parent->insert(obj);
    }
    if (!m_isLeaf) {
        QuadTree *child = getChild(obj->bound);
        if (child) {
            return child->insert(obj);
        }
    }
    return insert(obj);
}

void QuadTree::clear() {
    for (auto &obj : m_objects) {
        obj->qt = nullptr;
    }
    m_objects.clear();
    if (!m_isLeaf) {
        for (QuadTree *child : m_children) {
            child->clear();
        }
        m_isLeaf = true;
    }
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

void QuadTree::discardEmptyBuckets() {
    if (m_objects.size()) return;
    if (!m_isLeaf) {
        for (QuadTree *child : m_children)
            if (!child->m_isLeaf || child->m_objects.size()) return;
    }
    clear();
    if (m_parent != nullptr) m_parent->discardEmptyBuckets();
}

QuadTree *QuadTree::getChild(const Rect &bound) const {
    bool left = bound.x + bound.width < m_bound.getRight();
    bool right = bound.x > m_bound.getRight();

    if (bound.y + bound.height < m_bound.getTop()) {
        if (left) return m_children[1];   // Top left
        if (right) return m_children[0];  // Top right
    } else if (bound.y > m_bound.getTop()) {
        if (left) return m_children[2];   // Bottom left
        if (right) return m_children[3];  // Bottom right
    }
    return nullptr;  // Cannot contain boundary -- too large
}

bool QuadTree::isLeaf() const { return m_isLeaf; }

const std::array<QuadTree *, 4> &QuadTree::getChildren() const {
    return m_children;
}

}  // namespace SD
