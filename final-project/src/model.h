#include <vector>
#include "mesh.h"
#include <filesystem>


#ifndef MODEL_H
#define MODEL_H

class Model
{
public:
    Model(std::filesystem::path const& path);
    //Model(std::filesystem::path const& path, std::vector<Texture> textures);

    void draw(Shader& shader);
    void bindTexture(int slot, int location);

    Material material;
    //GPUMesh const getMeshes(int index) const;
private:
    std::vector<GPUMesh> m_meshes;
    void loadModel(std::filesystem::path const& path);
    //void loadModel(std::filesystem::path const& path, std::vector<Texture> textures);
};

#endif