#pragma once

#include "EntityManager.hpp"
#include "Components/RLComponent.hpp"

struct RLManager
{
    RLManager(EntityManager &manager, size_t width, size_t height);

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;

    auto update([[maybe_unused]] float delta_time) -> void;

    auto get_cell(size_t x, size_t y) -> Entity *;

    auto MDP_helper_state_value_function(int x, int y, uint8_t discount) -> int;
    auto MDP_state_value_function(uint8_t discount = 0.9) -> void;
    auto MDP_action_value_function(uint8_t discount = 0.9) -> void;

    auto get_neighbor(POLICY::POLICY action, int x, int y) -> Entity *;

private:
    EntityManager &manager;
    size_t width, height;
    Entity **grid;
};