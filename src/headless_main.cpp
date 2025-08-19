#include "core/Game.h"
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, char** argv) {
    bool headless = false;
    int ticks = 600; // 10 seconds at 60fps
    if (argc > 1 && std::string(argv[1]) == "--headless") headless = true;
    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a == "--ticks" && i+1<argc) { ticks = std::atoi(argv[++i]); }
    }
    if (!headless) { Game g; g.run(); return 0; }
    std::cerr << "Headless mode executing "<<ticks<<" ticks.\n";
    Game g; // TODO: introduce windowless Game variant
    const float dt = 1.f/60.f;
    for (int t=0; t<ticks; ++t) {
        g.step(dt);
    }
    nlohmann::json out; out["ticks_ran"] = ticks; out["dt"] = dt; out["status"] = "ok"; std::cout << out.dump(2) << "\n";
    return 0;
}
