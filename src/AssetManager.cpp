#include "AssetManager.h"

AssetManager::AssetManager(EntityManager *manager) : manager(manager)
{
}

void AssetManager::clear()
{
    textures.clear();
}

void AssetManager::add_texture(std::string texture_id, const char *texture_file)
{
    textures.emplace(texture_id, TextureManager::load_texture(texture_file));
}

SDL_Texture *AssetManager::get_texture(std::string texture_id)
{
    return textures[texture_id];
}