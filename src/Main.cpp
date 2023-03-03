#define SDL_MAIN_HANDLED

#include "Engine.hpp"
#include "Game.hpp"


int main([[maybe_unused]] int argc, [[maybe_unused]] char *arg[])
{

    // print out the current working directory
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    Game game = Game();

    game.init(770, 580);

    Engine::ticks_last_frame = Clock::now();

    while (game.is_running())
    {
        Engine::calculate_deltatime_and_wait();

        game.process_input();
        game.update(Engine::delta_time);
        game.render();
    }

    return 0;
}
