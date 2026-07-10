#include "./ResourceManager.h"
#include "spdlog/spdlog.h"
#include <SDL3_image/SDL_image.h>

ResourceManager::ResourceManager(){
    spdlog::info("Resource manager constructor called");
}

ResourceManager::~ResourceManager(){
    clear();
    spdlog::info("Resource manager destructor called");
}

void ResourceManager::clear(){
    for(auto texture:textures){
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void ResourceManager::addTexture(SDL_Renderer* renderer, const std::string& key, const std::string& filePath){
    SDL_Surface * surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);

    textures.emplace(key, texture);

    spdlog::info("New texture added to resources with key {}", key);
}

SDL_Texture* ResourceManager::getTexture(const std::string& key){
    return textures[key];
}