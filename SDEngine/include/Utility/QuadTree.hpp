#ifndef SD_QUAD_TREE_HPP
#define SD_QUAD_TREE_HPP

#include "Utility/Base.hpp"
#include "Utility/Math.hpp"

#include <any>
#include <cstdint>
#include <vector>
#include <array>

namespace SD {

class QuadTree;

struct SD_UTILITY_API Collidable {
   public:
    Rect bound;
    std::any data;

    Collidable();
    Collidable(const Rect &rect, std::any data);

   private:
    QuadTree *qt;

    friend class QuadTree;
};

class SD_UTILITY_API QuadTree {
   public:
    QuadTree();
    QuadTree(const Rect &bound, uint32_t capacity, uint32_t maxLevel);
    ~QuadTree();

    bool Insert(Collidable *obj);
    bool Remove(Collidable *obj);
    bool Update(Collidable *obj);
    void Clear();

    bool IsLeaf() const;
    const std::array<QuadTree *, 4> &GetChildren() const;

   private:
    void Subdivide();
    void DiscardEmptyBuckets();
    QuadTree *GetChild(const Rect &bound) const;

    bool m_isLeaf;
    Rect m_bound;
    uint32_t m_level;
    uint32_t m_capacity;
    uint32_t m_maxLevel;
    QuadTree *m_parent;
    std::array<QuadTree *, 4> m_children;
    std::vector<Collidable *> m_objects;
    std::vector<Collidable *> m_foundObjects;
};

}  // namespace SD

#endif /* SD_QUAD_TREE_HPP */
