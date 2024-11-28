cmake_minimum_required(VERSION 3.10)
project(SpaceRangers VERSION 1.0.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
# Ensure that the C++ standard is strictly followed
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable compiler-specific extensions
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set compiler flags for optimization and warnings
add_compile_options(-O2 -Wall -Wextra -Wno-unused-parameter)

# Find required external libraries
find_package(SFML COMPONENTS graphics REQUIRED)

# Add the internal libraries
add_subdirectory(sfui)
add_subdirectory(sfui/dependencies/CxxUtils)
add_subdirectory(sfui/dependencies/libvfs)

# Specify the source files
add_executable(SpaceRangers
    main.cpp
)

# Link libraries to the executable
target_link_libraries(SpaceRangers PRIVATE
    CxxUtils
    sfui
)
