#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <random>

class SoundManager {
public:
    SoundManager();
    // load (or get cached) buffer
    sf::SoundBuffer& buffer(const std::string& path);

    // play a sound (creates a new sf::Sound instance owned internally)
    void play(const std::string& path, float volume = 100.f, float pitch = 1.f);
    void playRandomPitch(const std::string& path, float volume = 100.f, float pitchMin = 0.95f, float pitchMax = 1.05f);

    void update(); // purge stopped sounds
private:
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers;
    std::vector<std::unique_ptr<sf::Sound>> active;
    std::mt19937 rng;
};