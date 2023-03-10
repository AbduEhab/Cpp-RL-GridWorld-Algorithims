#pragma once

#include "Component.hpp"

namespace POLICY
{
    enum POLICY
    {
        UP = 0,
        RIGHT = 1,
        DOWN = 2,
        LEFT = 3,
        NONE = 4
    };
}

namespace CELL_TYPE
{
    enum CELL_TYPE
    {
        BLANK = 0,
        DISABLED = 1,
        DOWN = 2,
        GOAL = 3,
        LEFT = 4,
        NONO = 5,
        OMNI = 6,
        RIGHT = 7,
        UP = 8,
        WALL = 9
    };
}

struct RLComponent : public Component
{
    int reward = 0;

    // POLICY::POLICY policy = POLICY::UP;

    CELL_TYPE::CELL_TYPE type = CELL_TYPE::BLANK;

    int state_number;

    float probability[4] = {0, 0, 0, 0}; // 0: up, 1: right, 2: down, 3: left

    POLICY::POLICY policy = POLICY::UP; // 0: up, 1: right, 2: down, 3: left

    RLComponent(int state_number, int reward, CELL_TYPE::CELL_TYPE type = CELL_TYPE::BLANK, float up = 0.25, float right = 0.25, float down = 0.25, float left = 0.25, POLICY::POLICY policy = POLICY::UP)
        : state_number(state_number), reward(reward), type(type)
    {
        probability[0] = up;
        probability[1] = right;
        probability[2] = down;
        probability[3] = left;

        this->policy = policy;
    }

    void init()
    {
    }

    void update([[maybe_unused]] float delta_time)
    {
    }

    void render()
    {
    }

    std::string to_string()
    {
        return std::string("Component<RLComponent>: ");
    }

    // == operator overload
    bool operator==(const RLComponent &other) const
    {
        return reward == other.reward && policy == other.policy && type == other.type && probability[0] == other.probability[0] && probability[1] == other.probability[1] && probability[2] == other.probability[2] && probability[3] == other.probability[3];
    }
};