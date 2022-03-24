#include "model.h"
#include "transform.h"
#include <list>
#include <memory>


#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject : public Model
{
	public:
		std::list<std::unique_ptr<GameObject>> children;
		GameObject* parent = nullptr;
		
		Transform transform;
		Texture rgbaTexture;
		Texture baseColorTexture;
		Texture specularTexture;
		
		// constructor, expects a filepath to a 3D model.
		
		GameObject();
		GameObject(std::filesystem::path const& path);
		GameObject(std::filesystem::path const& path, std::filesystem::path const& rgba);
		GameObject(std::filesystem::path const& path, 
				   std::filesystem::path const& baseColorTexture, 
			       std::filesystem::path const& specularTexture);

		//Add child. Argument input is argument of any constructor that you create.
		//By default you can use the default constructor and don't put argument input.
		/*template<typename... TArgs>
		void addChild(const TArgs&... args);*/

		void addChild(std::filesystem::path const& path);

		//Update transform if it was changed
		void updateSelfAndChild();
		// Force update of transform even if local space don't change
		void forceUpdateSelfAndChild();

		int getBaseColorTexture();
		int getSpecularTexture();
		int getRgbaTexture();
};

#endif