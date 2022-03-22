#include "model.h"
#include <iostream>

Model::Model(std::filesystem::path const& path)
{
	loadModel(path);
}

void Model::draw(Shader& shader)
{
	for (GPUMesh &mesh : m_meshes)
		mesh.draw(shader);
}

void Model::bindTexture(int slot, int location)
{
	for (GPUMesh& mesh : m_meshes)
		mesh.bindTexture(slot, location);
}


void Model::loadModel(std::filesystem::path const& path)
{
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.path().extension() == ".obj")
		{
			std::cout << entry.path() << "\n";
			m_meshes.push_back(GPUMesh{ entry.path() });
		}
	}
}

//void Model::loadModel(std::filesystem::path const& path, std::vector<Texture> textures)
//{
//	for (const auto& entry : std::filesystem::directory_iterator(path))
//	{
//		if (entry.path().extension() == ".obj")
//		{
//			
//			m_meshes.push_back(GPUMesh{ entry.path() });
//		}
//	}
//}