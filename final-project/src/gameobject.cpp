#include "gameobject.h"

GameObject::GameObject()
    : Model(), rgbaTexture("resources/checkboard.png"), baseColorTexture("resources/checkboard.png"), specularTexture("resources/checkboard.png")
{}

GameObject::GameObject(std::filesystem::path const& path)
    : Model(path), rgbaTexture("resources/checkboard.png"), baseColorTexture("resources/checkboard.png"), specularTexture("resources/checkboard.png")
{}

GameObject::GameObject(std::filesystem::path const& path, std::filesystem::path const& rgbaTexture)
    : Model(path), rgbaTexture(rgbaTexture, true), baseColorTexture("resources/checkboard.png"), specularTexture("resources/checkboard.png")
{}

GameObject::GameObject(std::filesystem::path const& path, 
                        std::filesystem::path const& baseColorTexture,
                        std::filesystem::path const& specularTexture)
    : Model(path), rgbaTexture("resources/checkboard.png"), baseColorTexture(baseColorTexture), specularTexture(specularTexture)
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

int GameObject::getBaseColorTexture() {
    return baseColorTexture.getTextureID();
}

int GameObject::getSpecularTexture() {
    return specularTexture.getTextureID();
}

int GameObject::getRgbaTexture() {
    return rgbaTexture.getTextureID();
}