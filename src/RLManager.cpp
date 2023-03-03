#include "RLManager.hpp"
#include <Components/SpriteComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Engine.hpp>

RLManager::RLManager(EntityManager &manager, size_t width, size_t height)
    : manager(manager), width(width), height(height)
{
    grid = new Entity *[width * height];

    // scale bloack size to fit screen
    size_t block_size = std::min(Engine::width / width, Engine::height / height);

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            Entity &block(manager.add_entity(std::string("Cell").append(" (").append(std::to_string(x)).append(", ").append(std::to_string(y)).append(")")));

            block.add_component<TransformComponent>(x * 32, y * 32, 0, 0, 32, 32, block_size / 32);
            block.add_component<SpriteComponent>("blank");

            grid[y * width + x] = &block;
        }
    }
}

auto RLManager::get_width() const -> size_t
{
    return width;
}

auto RLManager::get_height() const -> size_t
{
    return height;
}

auto RLManager::update([[maybe_unused]] float delta_time) -> void
{
    size_t block_size = std::min(Engine::width / (width * 32), Engine::height / (height * 32));

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            Entity &block = *grid[y * width + x];

            auto ts = block.get_component<TransformComponent>();

            // scale block's initial coords and final coords to fit screen
            ts->position.x = (float)x * 32 * block_size;
            ts->position.y = (float)y * 32 * block_size;
            ts->width = 32 * block_size;
            ts->height = 32 * block_size;
        }
    }
}

auto RLManager::get_cell(size_t x, size_t y) -> Entity *
{
    return grid[y * width + x];
}

auto RLManager::MDP_helper_state_value_function(int x, int y, uint8_t discount) -> int
{
    return 0;
    // auto cell = get_cell(x, y);

    // if (cell->get_component<RLComponent>()->type == CELL_TYPE::WALL)
    //     return 0;

    // auto up = get_neighbor(POLICY::UP, x, y);
    // auto right = get_neighbor(POLICY::RIGHT, x, y);
    // auto down = get_neighbor(POLICY::DOWN, x, y);
    // auto left = get_neighbor(POLICY::LEFT, x, y);

    // auto up_comp = up->get_component<RLComponent>();
    // auto right_comp = right->get_component<RLComponent>();
    // auto down_comp = down->get_component<RLComponent>();
    // auto left_comp = left->get_component<RLComponent>();

    // int up_value = up_comp->probability[0] * (up_comp->reward + discount * MDP_helper_state_value_function(*up, discount)) +
    //                up_comp->probability[1] * (right_comp->reward + discount * MDP_helper_state_value_function(*right, discount)) +
    //                up_comp->probability[2] * (down_comp->reward + discount * MDP_helper_state_value_function(*down, discount)) +
    //                up_comp->probability[3] * (left_comp->reward + discount * MDP_helper_state_value_function(*left, discount));

    // if (cell.get_component<RLComponent>()->type == CELL_TYPE::GOAL)
    // {
    //     return 0;
    // }
    // else if (cell.get_component<RLComponent>()->type == CELL_TYPE::WALL)
    // {
    //     return 0;
    // }
    // else if (cell.get_component<RLComponent>()->type == CELL_TYPE::NONO)
    // {
    //     return -500;
    // }
    // else
    // {
    //     return cell.get_component<RLComponent>()->reward + discount * MDP_helper_state_value_function(discount);
    // }
}

auto RLManager::MDP_state_value_function(uint8_t discount) -> void
{
    // for (size_t y = 0; y < height; y++)
    // {
    //     for (size_t x = 0; x < width; x++)
    //     {
    //         MDP_helper_state_value_function(x, y);
    //     }
    // }
}

auto RLManager::MDP_action_value_function(uint8_t discount) -> void
{
}

auto RLManager::get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *
{
    return nullptr;
    // switch (action)
    // {
    // case POLICY::UP:
    //     return x == 0 ? grid[y * width + x] : (grid[y * width + x - 1]->get_component<RLComponent>()->type == CELL_TYPE::WALL ? grid[y * width + x] : grid[y * width + x - 1]);
    // case POLICY::RIGHT:
    //     return x == width - 1 ? grid[y * width + x] : (grid[y * width + x + 1]->get_component<RLComponent>()->type == CELL_TYPE::WALL ? grid[y * width + x] : grid[y * width + x + 1]);
    // case POLICY::DOWN:
    //     return y == height - 1 ? grid[y * width + x] : (grid[(y + 1) * width + x]->get_component<RLComponent>()->type == CELL_TYPE::WALL ? grid[y * width + x] : grid[(y + 1) * width + x]);
    // case POLICY::LEFT:
    //     return y == 0 ? grid[y * width + x] : (grid[(y - 1) * width + x]->get_component<RLComponent>()->type == CELL_TYPE::WALL ? grid[y * width + x] : grid[(y - 1) * width + x]);
    // default:
    //     return grid[y * width + x];
    // }
}