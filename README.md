# SFML Game Framework

A compact 2D top‑down pixel-game framework built with C++ and SFML (inspired by Stardew Valley).

This repository contains a working skeleton and many gameplay systems implemented so you can iterate quickly:
- core game loop and state system
- input handling
- resource manager (textures, fonts)
- basic entity system (Player, NPC, ItemEntity, Crop)
- tilemap with simple collision
- inventory system (logic only)
- dialog manager
- quest system (collect-item example)
- save/load skeleton using nlohmann::json
- simple animation support (AnimatedSprite)

---

## Quick start (macOS / Linux / Windows)

Prerequisites
- CMake >= 3.14
- A C++17-capable compiler (clang, gcc, MSVC)
- SFML 3.x (Graphics, Window, System, Audio)
  - Install via your OS package manager or build from source.
- (optional) git

Note about dependencies
- The project uses FetchContent to automatically provide nlohmann/json when not available on the system.
- The project assumes a font file at `assets/fonts/arial.ttf` (or any TTF you prefer). See "Assets" below.

Build (recommended)
1. From the project root create and enter a build directory:

   mkdir -p build
   cd build

2. Configure with CMake (zsh/bash):

   cmake ..

   If CMake cannot find SFML, set the search path:

   cmake -DSFML_DIR=/path/to/sfml/lib/cmake/SFML ..

   or set CMAKE_PREFIX_PATH:

   cmake -DCMAKE_PREFIX_PATH=/path/to/sfml ..

3. Build:

   cmake --build . --config Release

4. Run the built executable (example path):

   ./bin/sfml-game-framework

Assets and working directory
- CMake is configured to copy `assets/` into the build directory after a successful build. Ensure you have `assets/fonts/arial.ttf` in the source `assets/` folder so UI text renders correctly.
- If the game cannot find the font, dialogs will still run but text will not be visible. See `assets/fonts/README.txt` for details.

---

## Controls / How to test
- Movement: W/A/S/D (or arrow keys)
- Interact: E (press when near an entity)
- Mouse: left-click to interact with interactive entities
- Save: press F5 (if wired in build) — or trigger from the code `PlayState::saveGame("save.json")`
- Load: press F9 (if wired) — or call `PlayState::loadGame("save.json")`

What to try
- Move the player around the tilemap and verify collision.
- Approach an item on the ground and press E or click it to pick it up.
- Plant/harvest crop tiles (crop entity is implemented; see spawn code in `PlayState`).
- Start dialog with NPCs (approach and press E).
- Observe the quest progress when picking up quest items (collect‑item example).

---

## Project layout (important files)

- CMakeLists.txt — project configuration, SFML find logic, and assets post-build copy
- src/main.cpp — program entry and Game bootstrap
- src/core/
  - Game.h / Game.cpp — main loop, window, top-level managers
  - State.h — base class for game states
- src/input/
  - InputManager.h / .cpp — centralizes keyboard/mouse polling
- src/resources/
  - ResourceManager.h / .cpp — caches textures & fonts
- src/entities/
  - Entity.h — base class
  - Player.h / Player.cpp — player movement, inventory reference
  - NPC.h / NPC.cpp — simple NPC and dialog trigger
  - ItemEntity.h / ItemEntity.cpp — world item pickups
  - AnimatedSprite.h / AnimatedSprite.cpp — simple sprite animation helper
  - Crop.h / Crop.cpp — crop lifecycle (growth/harvest)
- src/world/
  - TileMap.h / TileMap.cpp — tile layout and collision checks
- src/systems/
  - Inventory.h / Inventory.cpp — inventory logic (no UI)
  - Dialog.h / Dialog.cpp — dialog manager + overlay
  - Quest.h / Quest.cpp — quest system (collect item sample)
  - SaveGame.h / SaveGame.cpp — save/load helper (JSON)
- assets/ — sprites, fonts and other runtime resources copied into build

---

## Developer notes & architecture

- State pattern: `State` subclasses (e.g., `PlayState`) implement `handleEvent`, `update`, and `draw`.
- Entities: polymorphic objects derived from `Entity` with `update()` and `draw()` hooks.
- Input: `InputManager` polls keyboard and mouse once per frame (helps decouple input handling from SFML event internals).
- Resources: `ResourceManager` caches textures and fonts and returns references for lifetime management.
- Save/Load: `SaveGame` uses nlohmann::json. The current implementation serializes player position and simple inventory info; inventory deserialization is minimal and should be extended.
- SFML 3 differences: code has been adapted to SFML 3.x API changes (VideoMode, Font loading, event handling). If you use SFML 2.x, expect compile changes.

---

## TODO / Known limitations
- Inventory UI: the inventory system exists in code but lacks an on-screen UI for management; implement a simple grid and toggle with a key (e.g., I).
- Inventory persistence: saving currently stores minimal item info. Implement an Item registry/factory to reconstruct Item instances during load.
- QuestManager ownership: the code exposes a small global accessor for quests; consider moving QuestManager into `Game` and providing a proper API to states.
- Animated sprites: `AnimatedSprite` exists but player/NPC spritesheets and animations need wiring.
- Robust input bindings: move to configurable key bindings rather than hard-coded keys.
- Tests and CI: add unit tests and a simple CI pipeline.

---

## Extending the project (tips)
- Add an `ItemRegistry` to map item IDs → Item factory functions for save/load.
- Implement an `InventoryUI` class that subscribes to the `Player` inventory and draws items, supports drag/drop and stacking.
- Replace the ad-hoc tilemap with Tiled (.tmx) loader for richer maps.
- Add audio events: SFML Audio wrappers are simple to integrate with a `SoundManager`.

---

## Adding textures & sprites

To add visual content to the world, place image files (PNG recommended for pixel art) under `assets/textures/` (or a subfolder like `assets/textures/tiles/` or `assets/textures/entities/`). The project's CMake already copies the entire `assets/` directory into the build output, so files placed there are available at runtime without extra configuration.

Best practices:
- Use power-of-two or consistent tile sizes for tilemaps (e.g., 16x16 or 32x32) to simplify collisions and animation frames.
- Keep separate folders for tiles, characters, items and UI to avoid path confusion.
- Prefer PNGs with indexed palettes for pixel-art look and smaller sizes.

Loading textures at runtime

The project exposes a `ResourceManager` (see `src/resources/ResourceManager.*`) that caches textures and fonts. Use it from any class that has access to a `Game&` reference or pointer:

```cpp
// Load (or retrieve cached) texture from resources
sf::Texture& tex = game.resources().texture("assets/textures/entities/player_idle.png");

// Create a sprite using the texture
sf::Sprite sprite(tex);
sprite.setPosition(100.f, 150.f);

// In your draw() or render method
window.draw(sprite);
```

Notes:
- `ResourceManager::texture()` will throw or assert if the file is missing; make sure the path is correct relative to the project root. When running from the build directory the copied `assets/` folder is used, so the same `assets/...` path works.
- The manager caches loaded textures by path, so calling `texture()` repeatedly with the same path returns the same `sf::Texture&` without reloading from disk.

Using textures for tilemaps

For tilemaps, keep a single tileset image and map tile indices to sub-rects of the tileset using `sf::IntRect` when creating `sf::Sprite` instances or `sf::VertexArray` quads.

Using AnimatedSprite

The project includes an `AnimatedSprite` helper (`src/entities/AnimatedSprite.*`). It expects a spritesheet texture and frame rectangles. Example (pseudo):

```cpp
sf::Texture& sheet = game.resources().texture("assets/textures/entities/player_spritesheet.png");
AnimatedSprite anim(sheet);
anim.addAnimation("walk_down", frameRects, 0.12f); // frameRects: vector<sf::IntRect>
anim.play("walk_down");

// in update:
anim.update(dt);
// in draw:
window.draw(anim.getSprite());
```

If you'd like, I can add a small example entity that loads a texture and draws a sprite, or a simple tileset loader for `TileMap`. Tell me which example you prefer and I'll add it to the repo.

---

## License
MIT — see LICENSE file.

If you want, I can also add a short contributing guide, a roadmap, or create an example save file and a small test map. Let me know which you prefer next.