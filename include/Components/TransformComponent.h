#pragma once

#include "../EntityManager.h"
#include "../Game.h"
#include <SDL.h>
#include <glm/glm.hpp>

class TransformComponent : public Component
{
public:
    glm::vec2 position;
    glm::vec2 velocity;

    int width;
    int height;
    int scale;

    TransformComponent(int pos_X, int pos_Y, int vel_X, int vel_Y, int width, int height, int scale)
        : position(glm::vec2(pos_X, pos_Y)), velocity(glm::vec2(vel_X, vel_Y)),
          width(width), height(height), scale(scale) {}

    void update(float delta_time)
    {
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;
    }

    std::string to_string()
    {
        return std::string("Component<TransformComponent>: (") + std::to_string(position.x) + std::string(", ") + std::to_string(position.y) + std::string(")");
    }
};