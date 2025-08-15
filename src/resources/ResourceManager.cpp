#include "ResourceManager.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <vector>

sf::Texture& ResourceManager::texture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) return *it->second;
    auto tex = std::make_unique<sf::Texture>();

    // Candidate paths to try (in order)
    std::vector<std::string> candidates;
    candidates.push_back(path);

#ifdef PROJECT_ROOT
    // Try PROJECT_ROOT + path (useful when running from build dir)
    candidates.push_back(std::string(PROJECT_ROOT) + "/" + path);
    // Also try build/assets relative to project root
    candidates.push_back(std::string(PROJECT_ROOT) + "/build/" + path);
    candidates.push_back(std::string(PROJECT_ROOT) + "/build/assets/" + path.substr(std::string("assets/").size()));
#endif

    bool loaded = false;
    std::string loadedFrom;
    for (const auto &p : candidates) {
        try {
            if (tex->loadFromFile(p)) {
                loaded = true;
                loadedFrom = p;
                break;
            }
        } catch (...) {
            // ignore and continue
        }
    }

    if (!loaded) {
        // Print helpful diagnostic listing attempted absolute paths
        std::cerr << "Failed to load image\n    Provided path: " << path << "\n";
        for (const auto &p : candidates) {
            try {
                std::filesystem::path fp(p);
                std::error_code ec;
                auto abs = std::filesystem::absolute(fp, ec);
                if (!ec) std::cerr << "    Absolute path: " << abs.string() << " -> " << (std::filesystem::exists(abs) ? "exists" : "missing") << "\n";
                else std::cerr << "    Absolute path: " << p << "\n";
            } catch (...) {
                std::cerr << "    Tried: " << p << "\n";
            }
        }
        std::cerr << "Using empty placeholder texture (no pixels).\n";
        // leave tex as default-constructed texture (may be empty)
    } else {
        tex->setSmooth(false);
    }

    auto& ref = *tex;
    textures[path] = std::move(tex);
    return ref;
}

sf::Font& ResourceManager::font(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end()) return *it->second;
    auto f = std::make_unique<sf::Font>();
    // SFML 3: Font::openFromFile replaces loadFromFile
    if (!f->openFromFile(path)) throw std::runtime_error("Failed to load font: " + path);
    auto& ref = *f;
    fonts[path] = std::move(f);
    return ref;
}