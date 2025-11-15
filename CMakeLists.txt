cmake_minimum_required(VERSION 3.10)
project(G_ENGAIN VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ENGAIN)

# ENGAIN core sources
set(ENGAIN_CORE_SOURCES
    ENGAIN/core/Logger.cpp
    ENGAIN/core/TimeManager.cpp
    ENGAIN/core/Window.cpp
    ENGAIN/core/Texture.cpp
    ENGAIN/core/Input.cpp
    ENGAIN/core/Font.cpp
)

# Game1 sources
set(GAME1_SOURCES
    SANDBOX/game1/main.cpp
    ${ENGAIN_CORE_SOURCES}
)

# Game2 sources
set(GAME2_SOURCES
    SANDBOX/game2/main.cpp
    ${ENGAIN_CORE_SOURCES}
)

# Game3 sources
set(GAME3_SOURCES
    SANDBOX/game3/main.cpp
    ${ENGAIN_CORE_SOURCES}
)

# Game4 sources
set(GAME4_SOURCES
    SANDBOX/game4/main.cpp
    ${ENGAIN_CORE_SOURCES}
)

# Create game executables
add_executable(game1 ${GAME1_SOURCES})
add_executable(game2 ${GAME2_SOURCES})
add_executable(game3 ${GAME3_SOURCES})
add_executable(game4 ${GAME4_SOURCES})

# Link libraries
target_link_libraries(game1 ${SDL2_LIBRARIES} SDL2_image SDL2_ttf stdc++fs)
target_link_libraries(game2 ${SDL2_LIBRARIES} SDL2_image SDL2_ttf stdc++fs)
target_link_libraries(game3 ${SDL2_LIBRARIES} SDL2_image SDL2_ttf stdc++fs)
target_link_libraries(game4 ${SDL2_LIBRARIES} SDL2_image SDL2_ttf stdc++fs)

# Set output directories
set_target_properties(game1 game2 game3 game4 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Compiler warnings
if(MSVC)
    target_compile_options(game1 PRIVATE /W4)
    target_compile_options(game2 PRIVATE /W4)
    target_compile_options(game3 PRIVATE /W4)
    target_compile_options(game4 PRIVATE /W4)
else()
    target_compile_options(game1 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(game2 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(game3 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(game4 PRIVATE -Wall -Wextra -pedantic)
endif()
