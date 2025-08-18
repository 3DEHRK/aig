# SFML Game Framework

## Overview
This project is a 2D top-down pixel game framework built using SFML, inspired by games like Stardew Valley. It provides a structured approach to game development, encapsulating core functionalities such as game state management, input handling, sprite management, and entity management.

## Project Structure
The project is organized into several directories:

- **src/**: Contains the source code for the game framework.
  - **core/**: Manages the game loop and state transitions.
  - **graphics/**: Handles sprite loading and management.
  - **input/**: Processes user input from keyboard and mouse.
  - **entities/**: Defines game entities, including the player.
  - **world/**: Manages the game world layout and tile maps.
  - **utils/**: Provides utility functions, such as logging.

- **include/**: Contains public headers for external usage.

- **CMakeLists.txt**: Configuration file for building the project with CMake.

## Getting Started

### Prerequisites
- CMake
- SFML library

### Building the Project
1. Clone the repository:
   ```
   git clone <repository-url>
   cd sfml-game-framework
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build .
   ```

### Running the Game
After building the project, you can run the game executable located in the build directory.

## Usage
The framework provides a base for creating your own game. You can extend the `Game`, `State`, `Entity`, and other classes to implement your game logic, graphics, and interactions.

## Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue for any suggestions or improvements.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Completed Features
- Core game loop & state management (PlayState + HiddenRealmState prototype)
- Resource & input managers (textures/fonts; keyboard/mouse polling)
- Entity framework (Player, NPC, HostileNPC, Crop, Rail, ItemEntity, Projectile, Altar, HiddenLocation placeholder)
- Inventory system (stacking add/remove) & basic dialog UI
- Planting & crop growth placeholder (seeds to crop stages)
- Basic combat prototype (projectiles vs hostile NPC, health/damage)
- Altar activation consuming item → hidden realm state transition (prototype)
- SoundManager skeleton (buffer caching & playback)