#pragma once

#include "../AssetManager.hpp"
#include "../TextureManager.hpp"
#include "Animation.hpp"
#include "Component.hpp"
#include "TransformComponent.hpp"
#include <SDL_ttf.h>

struct SpriteComponent : public Component
{
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
        {
            // this opens a font style and sets a size
            // TTF_Font *Sans = TTF_OpenFont("Sans.ttf", 24);

            // // this is the color in rgb format,
            // // maxing out all would give you the color white,
            // // and it will be your text's color
            // SDL_Color White = {255, 255, 255};

            // // as TTF_RenderText_Solid could only be used on
            // // SDL_Surface then you have to create the surface first
            // SDL_Surface *surfaceMessage =
            //     TTF_RenderText_Solid(Sans, "put your text here", White);

            // // now you can convert it into a texture
            // SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

            // SDL_Rect Message_rect; // create a rect
            // Message_rect.x = 0;    // controls the rect's x coordinate
            // Message_rect.y = 0;    // controls the rect's y coordinte
            // Message_rect.w = 100;  // controls the width of the rect
            // Message_rect.h = 100;  // controls the height of the rect

            // // (0,0) is on the top left of the window/screen,
            // // think a rect as the text's box,
            // // that way it would be very simple to understand

            // // Now since it's a texture, you have to put RenderCopy
            // // in your game loop area, the area where the whole code executes

            // // you put the renderer's name first, the Message,
            // // the crop size (you can ignore this if you don't want
            // // to dabble with cropping), and the rect which is the size
            // // and coordinate of your texture
            // SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

            // // Don't forget to free your surface and texture
            // SDL_FreeSurface(surfaceMessage);
            // SDL_DestroyTexture(Message);
        }
    }

    std::string to_string()
    {
        return std::string("Component<SpriteComponent>: ");
    }

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
};