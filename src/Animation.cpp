#pragma once

#include "Animation.hpp"

Animation::Animation()
{
}

Animation::Animation(int index, int frames, int speed)
{
    this->index = index;
    this->frames = frames;
    this->speed = speed;
    this->animation_index = 0;
    this->animation_speed = 0;
}