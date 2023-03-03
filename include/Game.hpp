#pragma once

#include "Constants.hpp"

#include <SDL.h>
#include <SDL_image.h>
// #include <SDL_ttf.h>

#include "AssetManager.hpp"
#include "Component.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

struct AssetManager;

struct Game
{
    Game() = default;

    static AssetManager *asset_manager;

    [[nodiscard]] bool is_running() const;
    static SDL_Renderer *renderer; // the sdl graphics renderer
    void load_level(int level_number) const;
    void init(int width, int height);
    void process_input();
    void update(float delta_time);
    void render();
    void destroy();

private:
    bool running = false;
    SDL_Window *window; // the sdl frame
    SDL_Event event;
};
