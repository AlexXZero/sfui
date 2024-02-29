cmake_minimum_required(VERSION 3.10)

# Find required external libraries
find_package(SFML COMPONENTS graphics REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(sfui STATIC
    FontLibrary.cpp
    UserInterface.cpp
    component/Label.cpp
    component/Window.cpp
    component/details/Node.cpp
    component/details/Geometry.cpp
    component/details/Handlers.cpp
    component/details/Container.cpp
    component/details/Parsers.cpp
)

# Link dependent libraries
target_link_libraries(sfui PRIVATE
    sfml-graphics
    nlohmann_json::nlohmann_json
)
