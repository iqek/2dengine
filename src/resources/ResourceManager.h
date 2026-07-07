#pragma once

#include <map>
#include <string>
#include <SDL3/SDL.h>

class ResourceManager {
private:
    std::map<std::string, SDL_Texture*> textures;
    // ++ fonts, audio ...

public:
    ResourceManager();
    ~ResourceManager();

    void clear();
    void addTexture(SDL_Renderer* renderer, const std::string& key, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& key);
};