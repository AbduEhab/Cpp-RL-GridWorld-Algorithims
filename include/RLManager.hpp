#pragma once

#include "Components/RLComponent.hpp"
#include "EntityManager.hpp"

struct RLManager
{
    RLManager(EntityManager &manager, size_t width, size_t height, std::string_view &level);
    ~RLManager();

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;

    auto update([[maybe_unused]] double delta_time) -> void;

    auto get_cell(size_t x, size_t y) -> Entity *;

    auto MDP_state_value_function(bool print = false) -> void;
    auto MDP_action_value_function(bool print = false) -> void;

    double discount = 0.9f;

private:
    auto populate_LS_system(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neigbors)[4]) -> void;
    auto MDP_check_action_validity(POLICY::POLICY action, int x, int y) -> const bool;
    auto calc_action_values(Entity *curr_entity, RLComponent *const (&comp)[4], Entity *const (&neighbors)[4]) -> void;
    auto MDP_are_any_goals_reachable(Entity *const (&entities)[4]) -> const POLICY::POLICY;
    auto get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *;

    int num_states = 0;

    // create matrix with the values
    double *matrix;

    int matrix_size;

    double *value_matrix;
    int value_matrix_size;

    // create vec;
    double *state_vector;

    int vector_size;

    EntityManager &manager;
    size_t width, height;
    Entity **grid;
};