#pragma once

#include "Components/RLComponent.hpp"
#include "EntityManager.hpp"
#include "Matrix.hpp"

// "RLManager<T, width, height>::RLManager(EntityManager &manager, std::string_view &level)
template <typename T = float, unsigned int width = 0, unsigned int height = 0>
struct RLManager
{
    RLManager(EntityManager &manager, std::string_view &level);
    ~RLManager();

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;

    auto update([[maybe_unused]] float delta_time) -> void;

    auto get_cell(size_t x, size_t y) -> Entity *;

    auto MDP_state_value_function() -> void;
    auto MDP_action_value_function() -> void;

    float discount = 0.9f;

private:
    auto populate_LS_system(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neigbors)[4]) -> void;
    auto MDP_check_action_validity(POLICY::POLICY action, int x, int y) -> const bool;
    auto MDP_are_any_goals_reachable(Entity *const (&entities)[4]) -> const POLICY::POLICY;
    auto inverse_matrix(float *matrix, int matrix_size) -> bool;
    auto solve() -> void;

    auto get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *;

    int num_states = 0;

    // create matrix with the values
    matrix<T, height, width> matrix;

    int matrix_size;

    // create vec;
    float *vector;

    int vector_size;

    EntityManager &manager;
    size_t width, height;
    Entity **grid;
};