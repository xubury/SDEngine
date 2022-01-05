#include "System/SkyboxSystem.hpp"
#include "ECS/Scene.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem() : System("SkyboxSystem") {
    const float skybox_vertices[] = {
        // front
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
        // back
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0};

    const uint32_t skybox_indices[] = {// front
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
    m_box_vao = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    auto vbo = VertexBuffer::Create(skybox_vertices, sizeof(skybox_vertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::Create(skybox_indices, 36, BufferIOType::STATIC);
    m_box_vao->AddVertexBuffer(vbo, layout);
    m_box_vao->SetIndexBuffer(ibo);
}

void SkyboxSystem::OnInit() {
    m_skybox_shader = asset->LoadAndGet<Shader>("shaders/skybox.glsl");
    m_skybox_id = asset->LoadAsset<Skybox>("skybox/test.skybox");
}

void SkyboxSystem::OnPush() {}

void SkyboxSystem::OnPop() {}

void SkyboxSystem::OnImGui() {
    ImGui::Begin("Skybox System");
    {
        std::string path = asset->GetAssetPath(m_skybox_id);
        ImGui::TextUnformatted("Skybox Path:");
        ImGui::InputText("##Path", path.data(), path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            m_file_dialog_open = true;
            m_file_dialog_info.type = ImGuiFileDialogType::OPEN_FILE;
            m_file_dialog_info.title = "Open File";
            m_file_dialog_info.file_name = "";
            m_file_dialog_info.directory_path = asset->GetRootPath();
        }
        if (ImGui::FileDialog(&m_file_dialog_open, &m_file_dialog_info)) {
            m_skybox_id =
                asset->LoadAsset<Skybox>(m_file_dialog_info.result_path);
        }
    }
    ImGui::End();
}

void SkyboxSystem::OnRender() {
    glm::vec3 pos = scene->GetCamera()->GetWorldPosition();
    glm::mat4 projection = scene->GetCamera()->GetViewPorjection() *
                           glm::translate(glm::mat4(1.0f), pos);
    m_skybox_shader->SetMat4("u_projection", projection);

    device->SetDepthfunc(DepthFunc::LESS_EQUAL);
    auto skybox = asset->Get<Skybox>(m_skybox_id);
    if (skybox) {
        m_skybox_shader->SetTexture("skybox", skybox->GetTexture());
    }

    device->SetFramebuffer(renderer->GetFramebuffer());
    device->DrawBuffer(renderer->GetFramebuffer(), 0);  // only draw colors
    renderer->Submit(*m_skybox_shader, *m_box_vao, MeshTopology::TRIANGLES,
                     m_box_vao->GetIndexBuffer()->GetCount(), 0);
    device->SetDepthfunc(DepthFunc::LESS);
}

}  // namespace SD
