cmake_minimum_required(VERSION 3.10)
project(sfui VERSION 1.0.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
# Ensure that the C++ standard is strictly followed
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable compiler-specific extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags for optimization and warnings
add_compile_options(-O2 -Wall -Wextra -Wno-unused-parameter)

# Find required external libraries
find_package(SFML COMPONENTS graphics REQUIRED)
find_package(nlohmann_json REQUIRED)

# Specify the source files
add_library(sfui SHARED
    src/ImageData.cpp
    src/AnimsLoader.cpp
    src/FontLibrary.cpp
    src/UserInterface.cpp
    src/component/Edit.cpp
    src/component/Label.cpp
    src/component/Image.cpp
    src/component/Window.cpp
    src/component/details/ComponentBase.cpp
    src/component/details/Component.cpp
    src/component/details/Geometry.cpp
    src/component/details/Handlers.cpp
    src/component/details/Parsers.cpp
)

# Specify the include directories for the library
target_include_directories(sfui
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/include/sfui
)

# Link dependent libraries
target_link_libraries(sfui PUBLIC
    sfml-graphics
    nlohmann_json::nlohmann_json
    CxxUtils
)
