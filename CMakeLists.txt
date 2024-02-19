cmake_minimum_required(VERSION 3.10)

# Find required external libraries
find_package(SFML COMPONENTS graphics REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(sfui STATIC
    UserInterface.cpp
    ComponentNode.cpp
    ComponentGeometry.cpp
    ComponentHandlers.cpp
    ComponentContainer.cpp
    Parsers.cpp
)

# Link dependent libraries
target_link_libraries(sfui PRIVATE
    sfml-graphics
    nlohmann_json::nlohmann_json
)
