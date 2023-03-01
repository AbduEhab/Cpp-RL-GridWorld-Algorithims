#pragma once

#include "Entity.h"

class Entity;

class Component
{
public:
    Entity *owner;
    virtual ~Component() {}
    virtual void init() {}
    virtual void update(float delta_time) = 0;
    virtual void render() {}

    virtual std::string to_string()
    {
        return std::string("Method Not Implemented");
    }
};