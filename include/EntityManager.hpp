#pragma once

#include "Component.hpp"
#include "Constants.hpp"
#include "Entity.hpp"

struct Entity;

struct EntityManager
{
    void update(float delta_time);
    void render() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool is_empty() const;
    [[nodiscard]] Entity &add_entity(std::string entity_name);
    [[nodiscard]] std::vector<Entity *> get_entities() const;
    void list_all_entities() const;
    bool clear();

private:
    std::vector<Entity *> entities;
    
};