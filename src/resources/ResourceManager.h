#pragma once

#include <map>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class ResourceManager {
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, TTF_Font*> fonts;
    // ++ audio ...

public:
    ResourceManager();
    ~ResourceManager();

    void clear();

    void addTexture(SDL_Renderer* renderer, const std::string& key, const std::string& filePath);
    SDL_Texture* getTexture(const std::string& key);

    void addFont(const std::string& key, const std::string& filePath, int fontSize);
    TTF_Font* getFont(const std::string& key);
};