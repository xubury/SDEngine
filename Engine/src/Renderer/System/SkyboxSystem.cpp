#include "Renderer/System/SkyboxSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem(RenderTarget *target)
    : System("Skybox"), m_target(target) {
    const float skyboxVertices[] = {
        // front
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
        // back
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0};

    const uint32_t skyboxIndices[] = {// front
                                      0, 1, 2, 2, 3, 0,
                                      // right
                                      1, 5, 6, 6, 2, 1,
                                      // back
                                      7, 6, 5, 5, 4, 7,
                                      // left
                                      4, 0, 3, 3, 7, 4,
                                      // bottom
                                      4, 5, 1, 1, 0, 4,
                                      // top
                                      3, 2, 6, 6, 7, 3};
    m_skybox = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    auto vbo = VertexBuffer::create(skyboxVertices, sizeof(skyboxVertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::create(skyboxIndices, 36, BufferIOType::STATIC);
    m_skybox->addVertexBuffer(vbo, layout);
    m_skybox->setIndexBuffer(ibo);

    m_skyboxShader = ShaderLibrary::instance().load("shaders/skybox.glsl");
}

void SkyboxSystem::onInit() {}

void SkyboxSystem::onDestroy() {}

void SkyboxSystem::onRender() {
    glm::vec3 pos = renderer->getCamera()->getWorldPosition();
    m_skyboxShader->setMat4("u_model", glm::translate(glm::mat4(1.0f), pos));

    m_skyboxShader->bind();
    renderer->device().setDepthfunc(DepthFunc::LESS_EQUAL);
    renderer->device().setCullFace(Face::FRONT);

    renderer->setRenderTarget(*m_target);
    renderer->submit(*m_skybox, MeshTopology::TRIANGLES,
                     m_skybox->getIndexBuffer()->getCount(), 0);
    renderer->device().setCullFace(Face::BACK);
    renderer->device().setDepthfunc(DepthFunc::LESS);
}

}  // namespace SD
