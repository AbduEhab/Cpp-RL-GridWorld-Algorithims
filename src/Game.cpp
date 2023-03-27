#include <iostream>

#include <Components/SpriteComponent.hpp>
#include <Components/TransformComponent.hpp>
#include <Engine.hpp>
#include <Game.hpp>
#include <RLManager.hpp>

EntityManager manager;
AssetManager *Game::asset_manager = new AssetManager(manager);
SDL_Renderer *Game::renderer;
RLManager *rl_manager;

[[nodiscard]] bool Game::is_running() const
{
    return running;
}

void Game::init(int width, int height) // init SDL
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Error init sdl." << std::endl;
        return;
    }
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "error with window." << std::endl;
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        std::cerr << "error with renderer" << std::endl;
        return;
    }

    load_level(0);

    running = true;
}

void Game::load_level([[maybe_unused]] int level_number) const
{
    asset_manager->clear();

    asset_manager->add_texture("blank", "../assets/sprites/blank.png");
    asset_manager->add_texture("disabled", "../assets/sprites/disabled.png");
    asset_manager->add_texture("down", "../assets/sprites/down.png");
    asset_manager->add_texture("goal", "../assets/sprites/goal.png");
    asset_manager->add_texture("left", "../assets/sprites/left.png");
    asset_manager->add_texture("nono", "../assets/sprites/nono.png");
    asset_manager->add_texture("omni", "../assets/sprites/omni.png");
    asset_manager->add_texture("right", "../assets/sprites/right.png");
    asset_manager->add_texture("up", "../assets/sprites/up.png");
    asset_manager->add_texture("wall", "../assets/sprites/wall.png");

    std::string_view level;
    TimePoint bench_time;
    int bench_iterations = 1'000'000;

    switch (level_number)
    {
    case 0:
        level = "0 0 0 g 0 w 0 t 0 0 0 0";
        rl_manager = new RLManager(manager, 4, 3, level);
        rl_manager->MDP_state_value_function(true);
        rl_manager->MDP_optimal_policy(true);

        bench_time = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < bench_iterations; i++)
        {
            rl_manager->MDP_state_value_function();
            rl_manager->MDP_optimal_policy(false, false);
        }

        std::cout << bench_iterations << " iterations done in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - bench_time).count() << "ms" << std::endl;

        break;
    case 1:
        level = "0 0 0 g 0 w 0 t 0 0 0 0";
        rl_manager = new RLManager(manager, 4, 3, level);
        rl_manager->MDP_itterative_state_value_function(1e-9, true);
        rl_manager->MDP_optimal_policy(true);

        bench_time = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < bench_iterations; i++)
        {
            rl_manager->MDP_itterative_state_value_function(0.5, false);
            rl_manager->MDP_optimal_policy(false, false);
        }

        std::cout << bench_iterations << " iterations done in: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - bench_time).count() << "ms" << std::endl;
        break;

    default:
        Entity &block(manager.add_entity("block_1"));
        block.add_component<TransformComponent>(30, 30, 60, 60, 20, 20, 1);
        block.add_component<SpriteComponent>("goal");
        break;
    }
}

void Game::process_input()
{
    // create the SDL event object
    SDL_PollEvent(&event); // tell SDL to track the event
    switch (event.type)    // get event type and switch on it
    {
    case SDL_QUIT:       // escape key on the window
        running = false; // break game loop
        break;

    case SDL_KEYDOWN: // if any key is pressed down

        event.key.keysym.sym = SDLK_ESCAPE;

        if (event.key.keysym.sym) // if this key is the Esc key
            running = false;
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            Engine::width = event.window.data1;
            Engine::height = event.window.data2;
            debug_print("Window resized to: ", Engine::width, "x", Engine::height);
        }
        break;
    }
}

TimePoint time_s = Clock::now();

void Game::update(float delta_time)
{

    manager.update(delta_time);

    rl_manager->update(delta_time);

    bool res = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - time_s).count() >= FRAME_TIME_TARGET;

    if (res) [[unlikely]]
    {
        SDL_SetWindowTitle(window, (std::to_string(1 / delta_time).append(" | ").append(std::to_string(delta_time))).append(" | ").append(std::to_string(SDL_GetTicks64())).c_str());
        // debug_print("Game Update: ", manager.get_entities()[0]->get_component<TransformComponent>()->to_string());
        time_s = Clock::now();
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); // set up the given renderer to render a specific color
    SDL_RenderClear(renderer);                         // clear back buffer with the specified color

    manager.render();

    SDL_RenderPresent(renderer); // swap back and front buffer
}

//  cleaning after myself
void Game::destroy()
{
    asset_manager->clear();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}