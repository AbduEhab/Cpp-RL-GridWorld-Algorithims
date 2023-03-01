#pragma once

#include "Constants.hpp"

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

struct Cell
{
    constexpr Cell() = default;

    constexpr Cell(uint8_t reward, CELL_TYPE::CELL_TYPE type = CELL_TYPE::BLANK, uint8_t up = 100, uint8_t right = 100, uint8_t down = 100, uint8_t left = 100, POLICY::POLICY policy = POLICY::UP)
        : reward(reward), policy(policy), type(type)
    {
        probability[0] = up;
        probability[1] = right;
        probability[2] = down;
        probability[3] = left;
    }

    int reward = 0;

    POLICY::POLICY policy = POLICY::UP;

    CELL_TYPE::CELL_TYPE type = CELL_TYPE::BLANK;

    uint8_t probability[4] = {0, 0, 0, 0}; // 0: up, 1: right, 2: down, 3: left
};

template <size_t GRID_WIDTH, size_t GRID_HEIGHT>
struct GridWorld
{
    constexpr GridWorld()
    {
        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
        {
            grid[i] = Cell();
        }
    }
    // constexpr GridWorld(uint32_t width, uint32_t height) : width(width), height(height)
    // {
    //     for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
    //     {
    //         grid[i] = Cell();
    //     }
    // }

    auto update([[maybe_unused]] float delta_time) -> void
    {
    }

    constexpr auto get_cell(size_t x, size_t y) -> Cell &
    {
        return grid[y * GRID_WIDTH + x];
    }

    constexpr auto get_width() const -> size_t
    {
        return width;
    }

    constexpr auto get_height() const -> size_t
    {
        return height;
    }

private:
    Cell grid[GRID_WIDTH * GRID_HEIGHT];
    size_t width = GRID_WIDTH, height = GRID_HEIGHT;
};