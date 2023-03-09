#include "RLManager.hpp"
#include <Components/SpriteComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <LinearSolver.hpp>

#include <Engine.hpp>

RLManager::RLManager(EntityManager &manager, size_t width, size_t height, std::string_view &level)
    : manager(manager), width(width), height(height)
{
    assert(width > 0 && height > 0);

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

    // scale bloack size to fit screen
    size_t block_size = std::min(Engine::width / width, Engine::height / height);

    size_t num_states = 0;

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            Entity &block(manager.add_entity(std::string("State").append(" (").append(std::to_string(x)).append(", ").append(std::to_string(y)).append(")")));

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
            block.add_component<RLComponent>(type == CELL_TYPE::GOAL || type == CELL_TYPE::WALL || type == CELL_TYPE::NONO ? num_states : num_states++, reward, type);

            grid[y * width + x] = &block;
        }
    }

    matrix_size = (int)std::pow(num_states, 2);
    vector_size = num_states;

    matrix = new double[matrix_size];
    std::cout << "State Matrix Size: " << matrix_size << std::endl;
    for (size_t i = 0; i < matrix_size; i++)
    {
        matrix[i] = 0;
    }

    state_vector = new double[vector_size];
    std::cout << "State Vector Size: " << vector_size << std::endl;
    for (size_t i = 0; i < vector_size; i++)
    {
        state_vector[i] = 0;
    }

    value_matrix_size = vector_size * 4;
    value_matrix = new double[value_matrix_size];
    for (size_t i = 0; i < value_matrix_size; i++)
    {
        value_matrix[i] = 0;
    }
}

RLManager::~RLManager()
{
    delete[] grid;
    delete[] matrix;
    delete[] state_vector;
}

auto RLManager::get_width() const -> size_t
{
    return width;
}

auto RLManager::get_height() const -> size_t
{
    return height;
}

auto RLManager::update([[maybe_unused]] double delta_time) -> void
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
    return grid[x * width + y];
}

auto RLManager::populate_LS_system(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neighbors)[4]) -> void
{
    auto curr_component = curr_entity->get_component<RLComponent>();
    auto curr_state_state_number = curr_component->state_number;

    double curr_element_value = 0;

    for (int i = 0; i < 4; i++)
    {
        if (neighbors[i] == nullptr)
            curr_element_value -= -(curr_component->probability[i] * discount);
        else
        {
            auto neighbor_component = neighbors[i]->get_component<RLComponent>();
            auto neighbor_state_number = neighbor_component->state_number;
            auto neighbor_type = neighbor_component->type;

            if (neighbor_type == CELL_TYPE::GOAL || neighbor_type == CELL_TYPE::NONO)
                state_vector[curr_state_state_number] += comp[i]->reward * comp[i]->probability[i];
            else
                matrix[curr_state_state_number * vector_size + neighbor_state_number] += -(0.25 * discount);
        }
    }

    matrix[curr_state_state_number * vector_size + curr_state_state_number] += 1 - curr_element_value;
}

auto RLManager::MDP_state_value_function(bool print) -> void
{
    if (width <= 1 && height <= 1)
        return;

    for (size_t x = 0; x < height; x++)
    {
        for (size_t y = 0; y < width; y++)
        {
            auto proxy_entity = grid[width * x + y];

            auto curr_component = proxy_entity->get_component<RLComponent>();

            if (curr_component->type == CELL_TYPE::GOAL || curr_component->type == CELL_TYPE::WALL || curr_component->type == CELL_TYPE::NONO) [[unlikley]]
            {
                continue;
            }

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

            // populate matrix and state_vector
            populate_LS_system(proxy_entity, entity_comp_array, entity_array);
        }
    }
    // solve LS system
    gauss_solve(matrix, state_vector, vector_size);

    // print state_vector
    if (print)
    {
        std::cout << "---State Values---" << std::endl;
        for (size_t y = 0; y < vector_size; y++)
        {
            std::cout << y << ": " << state_vector[y] << " " << std::endl;
        }
    }
}

auto RLManager::calc_action_values(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neighbors)[4]) -> void
{
    auto curr_component = curr_entity->get_component<RLComponent>();
    auto curr_state_state_number = curr_component->state_number;

    for (int i = 0; i < 4; i++)
    {
        if (neighbors[i] == nullptr)
            value_matrix[curr_state_state_number * 4 + i] += curr_component->reward + (discount * state_vector[curr_state_state_number]);
        else
        {
            auto neighbor_component = comp[i];
            auto neighbor_state_number = neighbor_component->state_number;

            value_matrix[curr_state_state_number * 4 + i] += neighbor_component->reward + (discount * state_vector[neighbor_state_number]);
        }
    }
}

auto RLManager::MDP_action_value_function(bool print) -> void
{
    if (width <= 1 && height <= 1)
        return;

    for (size_t x = 0; x < height; x++)
    {
        for (size_t y = 0; y < width; y++)
        {
            auto proxy_entity = grid[width * x + y];

            auto curr_component = proxy_entity->get_component<RLComponent>();

            if (curr_component->type == CELL_TYPE::GOAL || curr_component->type == CELL_TYPE::WALL || curr_component->type == CELL_TYPE::NONO) [[unlikley]]
            {
                continue;
            }

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

            // populate matrix and state_vector
            calc_action_values(proxy_entity, entity_comp_array, entity_array);
        }
    }

    if (print)
    {
        std::cout << "---Action Values---" << std::endl;
        std::cout << "States\t"
                  << "UP\t"
                  << "RIGHT\t"
                  << "DOWN\t"
                  << "LEFT" << std::endl;
        for (size_t i = 0; i < vector_size; i++)
        {
            std::cout << i << "\t";
            for (size_t j = 0; j < 4; j++)
            {
                std::cout << value_matrix[i * 4 + j] << "\t";
            }
            std::cout << std::endl;
        }
    }
}

auto RLManager::calc_state_values_itteratively(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neighbors)[4]) -> double
{
    auto curr_component = curr_entity->get_component<RLComponent>();
    auto curr_state_state_number = curr_component->state_number;

    double max = 0;
    double values[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; i++)
    {
        if (neighbors[i] == nullptr)
            values[i] = curr_component->reward + (state_vector[curr_state_state_number] * discount);
        else
        {
            auto neighbor_component = comp[i];
            auto neighbor_state_number = neighbor_component->state_number;
            auto neighbor_type = neighbor_component->type;

            if (neighbor_type == CELL_TYPE::GOAL || neighbor_type == CELL_TYPE::NONO)
                values[i] = neighbor_component->reward;
            else
                values[i] = neighbor_component->reward + (state_vector[neighbor_state_number] * discount);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        max = std::max(max, values[i]);
    }

    return max;
}

auto RLManager::MDP_itterative_state_value_function(double epsilon, bool print) -> void
{
    if (width <= 1 && height <= 1)
        return;

    while (true)
    {
        double error = 0;

        for (size_t x = 0; x < height; x++)
        {
            for (size_t y = 0; y < width; y++)
            {
                auto proxy_entity = grid[width * x + y];

                auto curr_component = proxy_entity->get_component<RLComponent>();

                if (curr_component->type == CELL_TYPE::GOAL || curr_component->type == CELL_TYPE::WALL || curr_component->type == CELL_TYPE::NONO) [[unlikley]]
                    continue;

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

                // populate matrix and state_vector
                auto state_val = calc_state_values_itteratively(proxy_entity, entity_comp_array, entity_array);

                error = std::max(error, std::abs(state_vector[curr_component->state_number] - state_val));

                state_vector[curr_component->state_number] = state_val;
            }
        }

        if (error < epsilon)
            break;
    }

    if (print)
    {
        std::cout << "---State Value Function---" << std::endl;

        std::cout << "States\t"
                  << "Value Fucntion" << std::endl;

        for (size_t i = 0; i < vector_size; i++)
        {
            std::cout << i << "\t";
            std::cout << state_vector[i] << std::endl;
        }
    }
}

auto RLManager::MDP_optimal_policy(bool print, bool update) -> void
{
    MDP_action_value_function(print);

    if (update)
    {
        for (size_t x = 0; x < height; x++)
        {
            for (size_t y = 0; y < width; y++)
            {
                auto entity = grid[width * x + y];

                auto curr_component = entity->get_component<RLComponent>();
                auto curr_sprite = entity->get_component<SpriteComponent>();

                if (curr_component->type == CELL_TYPE::GOAL || curr_component->type == CELL_TYPE::WALL || curr_component->type == CELL_TYPE::NONO) [[unlikley]]
                    continue;

                int direction = 0;
                double max = -std::numeric_limits<double>::infinity();

                for (size_t i = 0; i < 4; i++)
                {
                    auto val = value_matrix[curr_component->state_number * 4 + i];

                    if (val > max)
                    {
                        max = value_matrix[curr_component->state_number * 4 + i];
                        direction = i;
                    }
                }

                curr_component->policy = static_cast<POLICY::POLICY>(direction);

                switch (curr_component->policy)
                {
                case POLICY::UP:
                    curr_sprite->set_texture("up");
                    break;
                case POLICY::RIGHT:
                    curr_sprite->set_texture("right");
                    break;
                case POLICY::DOWN:
                    curr_sprite->set_texture("down");
                    break;
                case POLICY::LEFT:
                    curr_sprite->set_texture("left");
                    break;
                default:
                    break;
                }
            }
        }
    }
}

auto RLManager::get_neighbor(POLICY::POLICY action, size_t x, size_t y) -> Entity *
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

auto RLManager::MDP_check_action_validity(POLICY::POLICY action, size_t x, size_t y) -> const bool
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