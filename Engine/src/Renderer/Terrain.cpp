#include "Renderer/Terrain.hpp"
#include "Utility/Math.hpp"

namespace SD {

Terrain::Terrain(int gridSize, int vertexCount)
    : m_gridSize(gridSize), m_vertexCount(vertexCount) {
    m_mesh.SetPolygonMode(PolygonMode::LINE);
    GenerateMesh();
}

void Terrain::GenerateMesh() {
    std::vector<Vertex> vertices(m_vertexCount *m_vertexCount);
    std::vector<uint32_t> indices(6 * (m_vertexCount - 1) *
                                  (m_vertexCount - 1));
    int vertexPointer = 0;
    float halfSize = (m_vertexCount - 1) * 0.5f;
    for (float i = -halfSize; i <= halfSize; ++i) {
        for (float j = -halfSize; j <= halfSize; ++j) {
            vertices[vertexPointer].position.x = j * m_gridSize;
            vertices[vertexPointer].position.y = 0.f;
            vertices[vertexPointer].position.z = i * m_gridSize;
            vertices[vertexPointer].normal.x = 0.f;
            vertices[vertexPointer].normal.y = 1.0f;
            vertices[vertexPointer].normal.z = 0.f;
            vertices[vertexPointer].texCoord.x =
                (float)(j + halfSize) / (m_vertexCount - 1);
            vertices[vertexPointer].texCoord.y =
                (float)(i + halfSize) / (m_vertexCount - 1);
            vertexPointer++;
        }
    }
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
    m_mesh.SetVerices(vertices);
    m_mesh.SetIndices(indices);
    ComputeNormal();
    m_mesh.Update();
}

const Mesh &Terrain::GetMesh() const { return m_mesh; }

const Material &Terrain::GetMaterial() const { return m_material; }

int Terrain::GetGridSize() const { return m_gridSize; }

int Terrain::GetVertexCount() const { return m_vertexCount; }

const std::vector<Vertex> &Terrain::GetVertices() const {
    return m_mesh.GetVertices();
}

std::vector<Vertex> &Terrain::GetVertices() { return m_mesh.GetVertices(); }

void Terrain::SetGridSize(int gridSize) { m_gridSize = gridSize; }

void Terrain::SetVertexCount(int vertexCount) { m_vertexCount = vertexCount; }

float Terrain::Height(float localX, float localZ) const {
    float x = localX / m_gridSize + (m_vertexCount - 1) * 0.5;
    float z = localZ / m_gridSize + (m_vertexCount - 1) * 0.5;
    int x0 = std::floor(x);
    int z0 = std::floor(z);
    auto &vertices = GetVertices();
    float f00 = vertices[z0 * m_vertexCount + x0].position.y;
    float f10 = vertices[z0 * m_vertexCount + (x0 + 1)].position.y;
    float f01 = vertices[(z0 + 1) * m_vertexCount + x0].position.y;
    float f11 = vertices[(z0 + 1) * m_vertexCount + (x0 + 1)].position.y;
    float xCoord = x - x0;
    float zCoord = z - z0;

    float u, v, w;
    if (xCoord <= 1 - zCoord) {
        glm::vec2 a(0, 0);
        glm::vec2 b(1, 0);
        glm::vec2 c(0, 1);
        BaryCentric(a, b, c, glm::vec2(xCoord, zCoord), u, v, w);
        return f00 * u + f10 * v + f01 * w;
    } else {
        glm::vec2 a(1, 0);
        glm::vec2 b(1, 1);
        glm::vec2 c(0, 1);
        BaryCentric(a, b, c, glm::vec2(xCoord, zCoord), u, v, w);
        return f10 * u + f11 * v + f01 * w;
    }
}

void Terrain::ComputeNormal() {
    float offset = 0.3f;
    auto &vertices = GetVertices();
    for (int i = 1; i < m_vertexCount - 1; ++i) {
        float z = (i - (m_vertexCount - 1) / 2.f) * m_gridSize;
        for (int j = 1; j < m_vertexCount - 1; ++j) {
            float x = (j - (m_vertexCount - 1) / 2.f) * m_gridSize;
            float hL = Height(x - offset, z);
            float hR = Height(x + offset, z);
            float hD = Height(x, z - offset);
            float hU = Height(x, z + offset);
            glm::vec3 &normal = vertices[i * m_vertexCount + j].normal;
            normal.x = hL - hR;
            normal.y = 2.0 * offset;
            normal.z = hD - hU;
            normal = normalize(normal);
        }
    }
}

}  // namespace SD
