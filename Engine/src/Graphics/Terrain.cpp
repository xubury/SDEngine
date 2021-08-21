#include "Graphics/Terrain.hpp"
#include "Utils/Math.hpp"

namespace sd {

Terrain::Terrain(int gridSize, int vertexCount) : m_gridSize(gridSize), m_vertexCount(vertexCount) {
    m_mesh.setWireframe(true);
    generateMesh();
}

void Terrain::generateMesh() {
    m_vertices.resize(m_vertexCount * m_vertexCount);
    std::vector<uint32_t> indices(6 * (m_vertexCount - 1) *
                                  (m_vertexCount - 1));
    int vertexPointer = 0;
    float halfSize = (m_vertexCount - 1) * 0.5f;
    for (float i = -halfSize; i <= halfSize; ++i) {
        for (float j = -halfSize; j <= halfSize; ++j) {
            m_vertices[vertexPointer].position.x = j * m_gridSize;
            m_vertices[vertexPointer].position.y = 0.f;
            m_vertices[vertexPointer].position.z = i * m_gridSize;
            m_vertices[vertexPointer].normal.x = 0.f;
            m_vertices[vertexPointer].normal.y = 1.0f;
            m_vertices[vertexPointer].normal.z = 0.f;
            m_vertices[vertexPointer].texCoord.x =
                (float)(j + halfSize) / (m_vertexCount - 1);
            m_vertices[vertexPointer].texCoord.y =
                (float)(i + halfSize) / (m_vertexCount - 1);
            vertexPointer++;
        }
    }
    computeNormal();

    int indexPointer = 0;
    for (int gz = 0; gz < m_vertexCount - 1; ++gz) {
        for (int gx = 0; gx < m_vertexCount - 1; ++gx) {
            int topLeft = (gz * m_vertexCount) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * m_vertexCount) + gx;
            int bottomRight = bottomLeft + 1;
            indices[indexPointer++] = topLeft;
            indices[indexPointer++] = bottomLeft;
            indices[indexPointer++] = topRight;
            indices[indexPointer++] = topRight;
            indices[indexPointer++] = bottomLeft;
            indices[indexPointer++] = bottomRight;
        }
    }
    m_mesh.setVerices(m_vertices);
    m_mesh.setIndices(indices);
    m_mesh.init();
}

const Mesh &Terrain::getMesh() const { return m_mesh; }

const Material &Terrain::getMaterial() const { return m_material; }

int Terrain::getGridSize() const { return m_gridSize; }

int Terrain::getVertexCount() const { return m_vertexCount; }

void Terrain::setGridSize(int gridSize) { m_gridSize = gridSize; }

void Terrain::setVertexCount(int vertexCount) { m_vertexCount = vertexCount; }

float Terrain::height(float localX, float localZ) const {
    float x = localX / m_gridSize + (m_vertexCount - 1) * 0.5;
    float z = localZ / m_gridSize + (m_vertexCount - 1) * 0.5;
    int x0 = std::floor(x);
    int z0 = std::floor(z);
    float f00 = m_vertices[z0 * m_vertexCount + x0].position.y;
    float f10 = m_vertices[z0 * m_vertexCount + (x0 + 1)].position.y;
    float f01 = m_vertices[(z0 + 1) * m_vertexCount + x0].position.y;
    float f11 = m_vertices[(z0 + 1) * m_vertexCount + (x0 + 1)].position.y;
    float xCoord = x - x0;
    float zCoord = z - z0;

    float u, v, w;
    if (xCoord <= 1 - zCoord) {
        glm::vec2 a(0, 0);
        glm::vec2 b(1, 0);
        glm::vec2 c(0, 1);
        baryCentric(a, b, c, glm::vec2(xCoord, zCoord), u, v, w);
        return f00 * u + f10 * v + f01 * w;
    } else {
        glm::vec2 a(1, 0);
        glm::vec2 b(1, 1);
        glm::vec2 c(0, 1);
        baryCentric(a, b, c, glm::vec2(xCoord, zCoord), u, v, w);
        return f10 * u + f11 * v + f01 * w;
    }
}

void Terrain::computeNormal() {
    float offset = 0.3f;
    for (int i = 1; i < m_vertexCount - 1; ++i) {
        float z = (i - (m_vertexCount - 1) / 2.f) * m_gridSize;
        for (int j = 1; j < m_vertexCount - 1; ++j) {
            float x = (j - (m_vertexCount - 1) / 2.f) * m_gridSize;
            float hL = height(x - offset, z);
            float hR = height(x + offset, z);
            float hD = height(x, z - offset);
            float hU = height(x, z + offset);
            glm::vec3 &normal = m_vertices[i * m_vertexCount + j].normal;
            normal.x = hL - hR;
            normal.y = 2.0 * offset;
            normal.z = hD - hU;
            normal = normalize(normal);
        }
    }
}

}  // namespace sd
