#pragma once

#include "Components/RLComponent.hpp"
#include "EntityManager.hpp"

struct RLManager
{
    RLManager(EntityManager &manager, size_t width, size_t height, std::string_view &level);
    ~RLManager();

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;

    auto update([[maybe_unused]] float delta_time) -> void;

    auto get_cell(size_t x, size_t y) -> Entity *;

    auto MDP_state_value_function() -> void;
    auto MDP_action_value_function() -> void;

    float discount = 0.9f;

private:
    auto populate_LS_system(int x, int y, RLComponent *const (&comp)[4], Entity *const (&neigbors)[4], POLICY::POLICY goal) -> void;
    auto MDP_check_action_validity(POLICY::POLICY action, int x, int y) -> const bool;
    auto MDP_are_any_goals_reachable(Entity *const (&entities)[4]) -> const POLICY::POLICY;

    auto get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *;

    // create matrix with the values
    float *matrix;

    // create vec;
    float *vector;

    EntityManager &manager;
    size_t width, height;
    Entity **grid;
};