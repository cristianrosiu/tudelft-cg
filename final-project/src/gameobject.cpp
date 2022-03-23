#include "gameobject.h"

GameObject::GameObject()
    : Model()
{}

GameObject::GameObject(std::filesystem::path const& path)
    : Model(path)
{}

//template<typename... TArgs>
//void GameObject::addChild(const TArgs&... args)
void GameObject::addChild(std::filesystem::path const& path)
{
    children.emplace_back(std::make_unique<GameObject>(path));
    children.back()->parent = this;
}

void GameObject::updateSelfAndChild()
{
    if (transform.isDirty())
        forceUpdateSelfAndChild();
    else
    {
        for (auto&& child : children)
            child->updateSelfAndChild();
    }
}

void GameObject::forceUpdateSelfAndChild()
{
    if (parent)
        transform.updateModelMatrix(parent->transform.getModelMatrix());
    else
        transform.updateModelMatrix();

    transform.markDirty(false);
    for (auto&& child : children)
    {
        child->transform.markDirty(true);
        child->forceUpdateSelfAndChild();
    }

}