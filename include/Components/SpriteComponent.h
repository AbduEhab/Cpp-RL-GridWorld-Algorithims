#pragma once

#include "../AssetManager.h"
#include "../TextureManager.h"
#include "Animation.h"
#include "TransformComponent.h"

class SpriteComponent : public Component
{
private:
    TransformComponent *transform;
    SDL_Texture *texture;
    SDL_Rect source_rect;
    SDL_Rect destiation_rect;
    bool is_animated;
    int frame_count;
    int animation_speed;
    bool is_fixed;
    std::map<std::string, Animation> animations;
    std::string current_animation_name;
    unsigned int animation_index = 0;

public:
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    SpriteComponent(std::string texture_id, bool is_fixed = false)
    {
        set_texture(texture_id);

        is_animated = false;
        this->is_fixed = is_fixed;
    }

    SpriteComponent(std::string texture_id, int frame_count, int animation_speed, bool hasDirections, bool is_fixed)
    {
        is_animated = true;
        this->frame_count = frame_count;
        this->animation_speed = animation_speed;
        this->is_fixed = is_fixed;

        if (hasDirections)
        {
            Animation downAnimation = Animation(0, frame_count, animation_speed);
            Animation rightAnimation = Animation(1, frame_count, animation_speed);
            Animation leftAnimation = Animation(2, frame_count, animation_speed);
            Animation upAnimation = Animation(3, frame_count, animation_speed);

            animations.emplace("DownAnimation", downAnimation);
            animations.emplace("RightAnimation", rightAnimation);
            animations.emplace("LeftAnimation", leftAnimation);
            animations.emplace("UpAnimation", upAnimation);

            this->animation_index = 0;
            this->current_animation_name = "DownAnimation";
        }
        else
        {
            Animation singleAnimation = Animation(0, frame_count, animation_speed);
            animations.emplace("SingleAnimation", singleAnimation);
            this->animation_index = 0;
            this->current_animation_name = "SingleAnimation";
        }

        play(this->current_animation_name);
        set_texture(texture_id);
    }

    void play(std::string animationName)
    {
        frame_count = animations[animationName].frames;
        animation_index = animations[animationName].index;
        animation_speed = animations[animationName].animation_speed;
        current_animation_name = animationName;
    }

    void set_texture(std::string texture_id)
    {
        texture = Game::asset_manager->get_texture(texture_id);
    }

    void init()
    {
        transform = owner->get_component<TransformComponent>();
        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = transform->width;
        source_rect.h = transform->height;

        destiation_rect.x = (int)transform->position.x;
        destiation_rect.y = (int)transform->position.y;
        destiation_rect.w = transform->width * transform->scale;
        destiation_rect.h = transform->height * transform->scale;
    }

    void update([[maybe_unused]] float delta_time)
    {
        if (is_animated)
        {
            source_rect.x = source_rect.w * static_cast<int>((SDL_GetTicks64() / animation_speed) % frame_count);
        }

        source_rect.y = animation_index * transform->height;

        if (!is_fixed)
        {
            destiation_rect.x = (int)transform->position.x;
            destiation_rect.y = (int)transform->position.y;
            destiation_rect.w = transform->width * transform->scale;
            destiation_rect.h = transform->height * transform->scale;
        }

    }

    void render()
    {
        TextureManager::draw(texture, source_rect, destiation_rect, flip);
    }

    std::string to_string()
    {
        return std::string("Component<SpriteComponent>: ");
    }
};