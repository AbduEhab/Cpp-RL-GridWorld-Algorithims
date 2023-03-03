#include "RLManager.hpp"
#include <Components/SpriteComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <LinearSolver.hpp>

#include <Engine.hpp>

RLManager::RLManager(EntityManager &manager, size_t width, size_t height, std::string_view &level)
    : manager(manager), width(width), height(height)
{

    auto entity_opt = new char[width * height];

    {
        int i = 0;
        int j = 0;

        for (auto var : level)
        {
            if (var == '\n' || var == ' ')
            {
                j++;
                continue;
            }
            entity_opt[i++] = level[j++];
        }
    }

    grid = new Entity *[width * height];

    matrix = new float[width * (height - 1)];
    vector = new float[width * (height - 1)];

    // scale bloack size to fit screen
    size_t block_size = std::min(Engine::width / width, Engine::height / height);

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            Entity &block(manager.add_entity(std::string("Cell").append(" (").append(std::to_string(x)).append(", ").append(std::to_string(y)).append(")")));

            auto c = entity_opt[y * width + x];

            CELL_TYPE::CELL_TYPE type;
            std::string texture_id;
            int reward = 0;

            switch (c)
            {
            case '0':
                type = CELL_TYPE::OMNI;
                texture_id = "omni";
                break;
            case 'w':
                type = CELL_TYPE::WALL;
                texture_id = "wall";
                break;
            case 'g':
                type = CELL_TYPE::GOAL;
                texture_id = "goal";
                reward = 1;
                break;
            case 't':
                type = CELL_TYPE::NONO;
                texture_id = "nono";
                reward = -1;
                break;
            case 's':
                type = CELL_TYPE::BLANK;
                texture_id = "blank";
                break;
            default:
                type = CELL_TYPE::DISABLED;
                texture_id = "disabled";
                break;
            }

            block.add_component<TransformComponent>(x * 32, y * 32, 0, 0, 32, 32, block_size / 32);
            block.add_component<SpriteComponent>(texture_id);
            block.add_component<RLComponent>(reward, type);

            grid[y * width + x] = &block;
        }
    }
}

RLManager::~RLManager()
{
    delete[] grid;
    delete[] matrix;
    delete[] vector;
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

            ts->position.x = x * 32 * block_size;
            ts->position.y = y * 32 * block_size;
            ts->width = 32;
            ts->height = 32;
        }
    }
}

auto RLManager::get_cell(size_t x, size_t y) -> Entity *
{
    return grid[y * width + x];
}

auto RLManager::populate_LS_system(int x, int y, RLComponent *const (&comp)[4], Entity *const (&neigbors)[4], POLICY::POLICY goal) -> void
{
    // top left corner
    if (neigbors[0] == nullptr && neigbors[3] == nullptr) [[unlikley]]
    {

        matrix[x + y * width] += 1 - (0.5 * discount);

        if (neigbors[1] != nullptr)
        {
            if (goal == POLICY::RIGHT)
                vector[x + y * width] += comp[1]->reward * discount;

            matrix[x + (y + 1) * width] += -(0.25 * discount);
        }
        if (neigbors[2] != nullptr)
        {
            if (goal == POLICY::DOWN)
                vector[x + y * width] += comp[2]->reward * discount;

            matrix[(x + 1) + y * width] += -(0.25 * discount);
        }
    }
    // top right corner
    else if (neigbors[0] == nullptr && neigbors[1] == nullptr) [[unlikley]]
    {

        matrix[x + y * width] += 1 - (0.5 * discount);

        if (neigbors[2] != nullptr)
        {
            if (goal == POLICY::DOWN)
                vector[x + y * width] += comp[2]->reward * discount;

            matrix[(x + 1) + y * width] += -(0.25 * discount);
        }
        if (neigbors[3] != nullptr)
        {
            if (goal == POLICY::LEFT)
                vector[x + y * width] += comp[3]->reward * discount;

            matrix[x + (y - 1) * width] += -(0.25 * discount);
        }
    }
    // bottom right corner
    else if (neigbors[1] == nullptr && neigbors[2] == nullptr) [[unlikley]]
    {

        matrix[x + y * width] += 1 - (0.5 * discount);

        if (neigbors[3] != nullptr)
        {
            if (goal == POLICY::LEFT)
                vector[x + y * width] += comp[3]->reward * discount;

            matrix[x + (y - 1) * width] += -(0.25 * discount);
        }
        if (neigbors[0] != nullptr)
        {
            if (goal == POLICY::UP)
                vector[x + y * width] += comp[0]->reward * discount;

            matrix[(x - 1) + y * width] += -(0.25 * discount);
        }
    }
    // bottom left corner
    else if (neigbors[2] == nullptr && neigbors[3] == nullptr) [[unlikley]]
    {

        matrix[x + y * width] += 1 - (0.5 * discount);

        if (neigbors[0] != nullptr)
        {
            if (goal == POLICY::UP)
                vector[x + y * width] += comp[0]->reward * discount;

            matrix[(x - 1) + y * width] += -(0.25 * discount);
        }
        if (neigbors[1] != nullptr)
        {
            if (goal == POLICY::RIGHT)
                vector[x + y * width] += comp[1]->reward * discount;

            matrix[x + (y + 1) * width] += -(0.25 * discount);
        }
    }
    // if not edge
    else
    {
        matrix[x + y * width] += 1 - (0.25 * discount);

        if (neigbors[0] != nullptr)
        {
            if (goal == POLICY::UP)
                vector[x + y * width] += comp[0]->reward * discount;

            matrix[(x - 1) + y * width] += -(0.25 * discount);
        }
        if (neigbors[1] != nullptr)
        {
            if (goal == POLICY::RIGHT)
                vector[x + y * width] += comp[1]->reward * discount;

            matrix[x + (y + 1) * width] += -(0.25 * discount);
        }
        if (neigbors[2] != nullptr)
        {
            if (goal == POLICY::DOWN)
                vector[x + y * width] += comp[2]->reward * discount;

            matrix[(x + 1) + y * width] += -(0.25 * discount);
        }
        if (neigbors[3] != nullptr)
        {
            if (goal == POLICY::LEFT)
                vector[x + y * width] += comp[3]->reward * discount;

            matrix[x + (y - 1) * width] += -(0.25 * discount);
        }
    }
}

auto RLManager::MDP_state_value_function() -> void
{
    if (width <= 1 && height <= 1)
        return;

    for (size_t x = 0; x < height; x++)
    {
        for (size_t y = 0; y < width; y++)
        {
            // MDP_helper_state_value_function(x, y);

            auto is_up_possible = MDP_check_action_validity(POLICY::UP, x, y);
            auto is_right_possible = MDP_check_action_validity(POLICY::RIGHT, x, y);
            auto is_down_possible = MDP_check_action_validity(POLICY::DOWN, x, y);
            auto is_left_possible = MDP_check_action_validity(POLICY::LEFT, x, y);

            Entity *up = nullptr;
            Entity *right = nullptr;
            Entity *down = nullptr;
            Entity *left = nullptr;

            RLComponent *up_comp = nullptr;
            RLComponent *right_comp = nullptr;
            RLComponent *down_comp = nullptr;
            RLComponent *left_comp = nullptr;

            if (is_up_possible)
            {
                up = get_neighbor(POLICY::UP, x, y);
                up_comp = up->get_component<RLComponent>();
            }

            if (is_right_possible)
            {
                right = get_neighbor(POLICY::RIGHT, x, y);
                right_comp = right->get_component<RLComponent>();
            }

            if (is_down_possible)
            {
                down = get_neighbor(POLICY::DOWN, x, y);
                down_comp = down->get_component<RLComponent>();
            }

            if (is_left_possible)
            {
                left = get_neighbor(POLICY::LEFT, x, y);
                left_comp = left->get_component<RLComponent>();
            }

            Entity *const entity_array[4] = {up, right, down, left};
            RLComponent *entity_comp_array[4] = {up_comp, right_comp, down_comp, left_comp};

            auto goal = MDP_are_any_goals_reachable(entity_array);

            // populate matrix and vector
            populate_LS_system(x, y, entity_comp_array, entity_array, goal);
        }
    }

    // solve LS system
    gauss_solve(matrix, vector, width * (height - 1));

    // print vector
    for (size_t y = 0; y < width * (height - 1); y++)
    {
        std::cout << y << ": " << vector[y] << " " << std::endl;
    }
}

auto RLManager::MDP_action_value_function() -> void
{
}

auto RLManager::get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *
{
    switch (action)
    {
    case POLICY::UP:
        return grid[(x - 1) * width + y];
        break;
    case POLICY::RIGHT:
        return grid[x * width + (y + 1)];
        break;
    case POLICY::DOWN:
        return grid[(x + 1) * width + y];
        break;
    case POLICY::LEFT:
        return grid[x * width + (y - 1)];
        break;
    default:
        return nullptr;
        break;
    }
}

auto RLManager::MDP_check_action_validity(POLICY::POLICY action, int x, int y) -> const bool
{
    bool value = false;
    switch (action)
    {
    case POLICY::UP:
        if (x == 0)
            return false;
        else
        {
            auto g = grid[width * (x - 1) + y];
            auto type = g->get_component<RLComponent>()->type;

            if (type == CELL_TYPE::WALL)
                return false;
            else
                return true;
        }
        break;
    case POLICY::RIGHT:
        if (y == width - 1)
            return false;
        else
        {
            auto g = grid[width * x + (y + 1)];
            auto type = g->get_component<RLComponent>()->type;

            if (type == CELL_TYPE::WALL)
                return false;
            else
                return true;
        }
        break;
    case POLICY::DOWN:
        if (x == height - 1)
            return false;
        else
        {
            auto g = grid[width * (x + 1) + y];
            auto type = g->get_component<RLComponent>()->type;

            if (type == CELL_TYPE::WALL)
                return false;
            else
                return true;
        }
        break;
    case POLICY::LEFT:
        if (y == 0)
            return false;
        else
        {
            auto g = grid[width * x + (y - 1)];
            auto type = g->get_component<RLComponent>()->type;

            if (type == CELL_TYPE::WALL)
                return false;
            else
                return true;
        }
        break;
    }

    return value;
}

auto RLManager::MDP_are_any_goals_reachable(Entity *const (&entities)[4]) -> const POLICY::POLICY
{
    for (size_t i = 0; i < 4; i++)
    {
        if (entities[i] == nullptr)
            continue;

        if (entities[i]->get_component<RLComponent>()->type == CELL_TYPE::GOAL)
            return (POLICY::POLICY)i;
    }

    return POLICY::NONE;
}