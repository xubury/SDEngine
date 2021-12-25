#include "Renderer/SkyboxLoader.hpp"
#include "Utility/String.hpp"

namespace SD {

const std::unordered_map<std::string, CubeMapFace> SKYBOX_NAME_MAP = {
    {"Right", CubeMapFace::RIGHT}, {"Left", CubeMapFace::LEFT},
    {"Top", CubeMapFace::TOP},     {"Bottom", CubeMapFace::BOTTOM},
    {"Front", CubeMapFace::FRONT}, {"Back", CubeMapFace::BACK}};

inline Exception GetException(size_t line_number, const std::string& msg) {
    return Exception(fmt::format("Line {}: {}", line_number, msg));
}

Ref<void> SkyboxLoader::LoadAsset(const std::string& path) {
    Ref<Skybox> skybox = CreateRef<Skybox>();
    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {
        file.open(path);
    } catch (std::ifstream::failure& e) {
        throw FileException(path, std::strerror(errno));
    }
    file.exceptions(std::ifstream::badbit);
    ParseStream(file, std::filesystem::path(path).parent_path(), *skybox);
    return skybox;
}

void SkyboxLoader::ParseStream(std::istream& stream,
                               const std::filesystem::path& directory,
                               Skybox& skybox) {
    int line_number = 0;
    for (std::string line; std::getline(stream, line); ++line_number) {
        String::Trim(line);
        size_t sep_pos = line.find_first_of('=');
        if (sep_pos == std::string::npos)
            throw GetException(line_number, "Invalid line");
        std::string face_name = (line.substr(0, sep_pos));
        String::Trim(face_name);
        if (SKYBOX_NAME_MAP.count(face_name) == 0) {
            throw GetException(line_number, "Invalid face name");
        }
        CubeMapFace face = SKYBOX_NAME_MAP.at(face_name);
        std::string path = line.substr(sep_pos + 1);
        String::Trim(path);
        Ref<Bitmap> bitmap = Manager().LoadAndGet<Bitmap>(directory / path);
        if (bitmap) {
            skybox.SetFace(face, *bitmap);
        } else {
            throw GetException(line_number, "Invalid image path");
        }
    }
}

}  // namespace SD
