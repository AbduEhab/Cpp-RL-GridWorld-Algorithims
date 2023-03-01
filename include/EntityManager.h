#pragma once

#include "Component.h"
#include "Entity.h"

class Entity;

class EntityManager
{
private:
    std::vector<Entity *> entities;

public:
    void update(float delta_time);
    void render() const;
    [[NODISCARD]] size_t size() const;
    [[NODISCARD]] bool is_empty() const;
    [[NODISCARD]] Entity &add_entity(std::string entity_name);
    [[NODISCARD]] std::vector<Entity *> get_entities() const;
    void list_all_entities() const;
    bool clear();
};