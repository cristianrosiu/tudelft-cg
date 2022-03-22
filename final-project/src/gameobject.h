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
		
		// constructor, expects a filepath to a 3D model.
		
		GameObject(std::filesystem::path const& path);
		GameObject(std::filesystem::path const& path, std::vector<Texture> textures);

		//Add child. Argument input is argument of any constructor that you create.
		//By default you can use the default constructor and don't put argument input.
		template<typename... TArgs>
		void addChild(const TArgs&... args);

		//Update transform if it was changed
		void updateSelfAndChild();
		// Force update of transform even if local space don't change
		void forceUpdateSelfAndChild();
};

#endif