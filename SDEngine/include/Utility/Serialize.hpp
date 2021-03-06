#ifndef SD_SERIALIZE_HPP
#define SD_SERIALIZE_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <fstream>
#include <cereal/types/array.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/portable_binary.hpp>

#define SERIALIZE(...)               \
    template <typename Archive>      \
    void serialize(Archive& archive) \
    {                                \
        archive(__VA_ARGS__);        \
    }

namespace glm {

template <class Archive>
void serialize(Archive& archive, glm::vec2& v)
{
    archive(v.x, v.y);
}

template <class Archive>
void serialize(Archive& archive, glm::vec3& v)
{
    archive(v.x, v.y, v.z);
}

template <class Archive>
void serialize(Archive& archive, glm::vec4& v)
{
    archive(v.x, v.y, v.z, v.w);
}

template <class Archive>
void serialize(Archive& archive, glm::ivec2& v)
{
    archive(v.x, v.y);
}

template <class Archive>
void serialize(Archive& archive, glm::ivec3& v)
{
    archive(v.x, v.y, v.z);
}

template <class Archive>
void serialize(Archive& archive, glm::ivec4& v)
{
    archive(v.x, v.y, v.z, v.w);
}

template <class Archive>
void serialize(Archive& archive, glm::uvec2& v)
{
    archive(v.x, v.y);
}

template <class Archive>
void serialize(Archive& archive, glm::uvec3& v)
{
    archive(v.x, v.y, v.z);
}

template <class Archive>
void serialize(Archive& archive, glm::uvec4& v)
{
    archive(v.x, v.y, v.z, v.w);
}

template <class Archive>
void serialize(Archive& archive, glm::dvec2& v)
{
    archive(v.x, v.y);
}

template <class Archive>
void serialize(Archive& archive, glm::dvec3& v)
{
    archive(v.x, v.y, v.z);
}

template <class Archive>
void serialize(Archive& archive, glm::dvec4& v)
{
    archive(v.x, v.y, v.z, v.w);
}

// glm matrices serialization
template <class Archive>
void serialize(Archive& archive, glm::mat2& m)
{
    archive(m[0], m[1]);
}

template <class Archive>
void serialize(Archive& archive, glm::dmat2& m)
{
    archive(m[0], m[1]);
}

template <class Archive>
void serialize(Archive& archive, glm::mat3& m)
{
    archive(m[0], m[1], m[2]);
}

template <class Archive>
void serialize(Archive& archive, glm::mat4& m)
{
    archive(m[0], m[1], m[2], m[3]);
}

template <class Archive>
void serialize(Archive& archive, glm::dmat4& m)
{
    archive(m[0], m[1], m[2], m[3]);
}

template <class Archive>
void serialize(Archive& archive, glm::quat& q)
{
    archive(q.x, q.y, q.z, q.w);
}

template <class Archive>
void serialize(Archive& archive, glm::dquat& q)
{
    archive(q.x, q.y, q.z, q.w);
}

}  // namespace glm

#endif /* SD_SERIALIZE_HPP */
