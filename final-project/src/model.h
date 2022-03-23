#include <vector>
#include "mesh.h"
#include <filesystem>


#ifndef MODEL_H
#define MODEL_H

class Model
{
public:
    Model();
    Model(std::filesystem::path const& path);

    virtual void draw(Shader& shader);
    void bindTexture(int slot, int location);
    int const &getMeshLength();

    Material material;
protected:
    std::vector<GPUMesh> m_meshes;
private:
   
    void loadModel(std::filesystem::path const& path);
};

#endif