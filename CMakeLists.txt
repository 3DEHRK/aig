cmake_minimum_required(VERSION 3.10)
project(sfml-game-framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer user-installed SFML 3 (case-sensitive component names)
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# JSON (nlohmann) dependency: try system package first, then FetchContent fallback
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

include_directories(include)

# Collect all cpp files then filter out entrypoints to build shared list
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
set(COMMON_SOURCES)
foreach(f ${ALL_SOURCES})
    if(f MATCHES ".*/src/main.cpp" OR f MATCHES ".*/src/headless_main.cpp")
        # skip entrypoints
    else()
        list(APPEND COMMON_SOURCES ${f})
    endif()
endforeach()

add_executable(sfml-game-framework src/main.cpp ${COMMON_SOURCES})
add_executable(sfml-game-framework-headless src/headless_main.cpp ${COMMON_SOURCES})

foreach(tgt IN ITEMS sfml-game-framework sfml-game-framework-headless)
    target_link_libraries(${tgt}
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        nlohmann_json::nlohmann_json
    )
    set_target_properties(${tgt} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()