#ifndef SD_QUAD_TREE_HPP
#define SD_QUAD_TREE_HPP

#include <any>
#include <cstdint>
#include <vector>
#include "Utils/Math.hpp"

namespace sd {

class QuadTree;

struct Collidable {
   public:
    Rect bound;
    std::any data;

    Collidable();
    Collidable(const Rect &rect, std::any data);

   private:
    QuadTree *qt;

    friend class QuadTree;
};

class QuadTree {
   public:
    QuadTree();
    QuadTree(const Rect &bound, uint32_t capacity, uint32_t maxLevel);
    ~QuadTree();

    bool insert(Collidable *obj);
    bool remove(Collidable *obj);
    bool update(Collidable *obj);
    void clear();

   private:
    bool m_isLeaf;
    Rect m_bound;
    uint32_t m_level;
    uint32_t m_capacity;
    uint32_t m_maxLevel;
    QuadTree *m_parent;
    QuadTree *m_children[4];
    std::vector<Collidable *> m_objects;
    std::vector<Collidable *> m_foundObjects;
    void subdivide();
};

}  // namespace sd

#endif /* SD_QUAD_TREE_HPP */
