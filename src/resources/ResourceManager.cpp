#include "ResourceManager.h"
#include <stdexcept>
#include <iostream>
#include <SFML/Config.hpp>

sf::Texture& ResourceManager::texture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) return *it->second;
    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(path)) {
        std::cerr << "[ResourceManager] Missing texture: " << path << " -> using fallback placeholder." << '\n';
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
        sf::Image img({4u,4u}, sf::Color(255,0,255));
        for (unsigned y=0;y<4;++y) for (unsigned x=0;x<4;++x) if ((x+y)%2==0) img.setPixel({x,y}, sf::Color(40,40,40));
        tex->loadFromImage(img);
#else
        sf::Image img; img.create(4,4,sf::Color(255,0,255));
        for (unsigned y=0;y<4;++y) for (unsigned x=0;x<4;++x) if ((x+y)%2==0) img.setPixel(x,y,sf::Color(40,40,40));
        tex->create(4,4); tex->update(img);
#endif
        tex->setRepeated(true);
    }
    tex->setSmooth(false);
    auto &ref = *tex;
    textures[path] = std::move(tex);
    return ref;
}

sf::Font& ResourceManager::font(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end()) return *it->second;
    auto f = std::make_unique<sf::Font>();
    if (!f->openFromFile(path)) {
        static bool warned=false; if(!warned){ std::cerr << "[ResourceManager] Missing font: " << path << " -> using empty fallback.\n"; warned=true; }
        // font stays empty; SFML text with empty font may not render but avoids crash
    }
    auto &ref = *f;
    fonts[path] = std::move(f);
    return ref;
}