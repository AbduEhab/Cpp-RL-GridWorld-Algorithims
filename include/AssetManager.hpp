#pragma once

#include "EntityManager.hpp"
#include "TextureManager.hpp"
#include <map>

struct AssetManager
{
    AssetManager(EntityManager &manager);
    ~AssetManager();
    void clear();
    void add_texture(std::string texture_id, const char *asset_file);
    SDL_Texture *get_texture(std::string texture_id);

private:
    EntityManager &manager;
    std::map<std::string, SDL_Texture *> textures;
};
