#pragma once

#include "Constants.hpp"

#include <SDL.h>
#include <SDL_image.h>
// #include <SDL_ttf.h>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "EntityManager.h"

class AssetManager;

class Game
{
private:
    bool running = false;
    SDL_Window *window; // the sdl frame
    SDL_Event event;

public:
    Game() = default;

    static AssetManager *asset_manager;
    
    [[NODISCARD]] bool is_running() const;
    static SDL_Renderer *renderer; // the sdl graphics renderer
    void load_level(int level_number) const;
    void init(int width, int height);
    void process_input();
    void update(float delta_time);
    void render();
    void destroy();
};
