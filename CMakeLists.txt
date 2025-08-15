cmake_minimum_required(VERSION 3.14)
project(sfml-game-framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer user-installed SFML 3 (case-sensitive component names)
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

include_directories(include)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(sfml-game-framework ${SOURCES})

# expose project source directory to C++ code so assets can be loaded relative to repo root
if(NOT DEFINED PROJECT_ROOT)
    target_compile_definitions(sfml-game-framework PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
endif()

# Copy assets into the build directory so running the binary from the build dir finds them
add_custom_command(TARGET sfml-game-framework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Copying assets directory to build folder"
)

# Ensure minimum compatibility for fetched projects that expect older policy behavior
if(NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE INTERNAL "Minimum policy version for external projects")
endif()

# Provide nlohmann_json via FetchContent if not available on system
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# Link libraries

target_link_libraries(sfml-game-framework
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    nlohmann_json::nlohmann_json
)

set_target_properties(sfml-game-framework PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)