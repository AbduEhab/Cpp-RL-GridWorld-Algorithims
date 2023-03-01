#pragma once

struct Animation
{
    int index;
    int frames;
    int speed;
    int animation_speed;
    int animation_index;

    Animation();
    Animation(int index, int frames, int speed);
};