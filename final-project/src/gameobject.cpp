#include "gameobject.h"

GameObject::GameObject(std::filesystem::path const& path)
    : Model(path)
{}


GameObject::GameObject(std::filesystem::path const& path, std::vector<Texture> textures)
    : Model(path)
{}

template<typename... TArgs>
void GameObject::addChild(const TArgs&... args)
{
    children.emplace_back(std::make_unique<GameObject>(args...));
    children.back()->parent = this;
}

void GameObject::updateSelfAndChild()
{
    if (!transform.isDirty())
        return;

    forceUpdateSelfAndChild();
}

void GameObject::forceUpdateSelfAndChild()
{
    if (parent)
        transform.updateModelMatrix(parent->transform.getModelMatrix());
    else
        transform.updateModelMatrix();

    for (auto&& child : children)
        child->forceUpdateSelfAndChild();

}