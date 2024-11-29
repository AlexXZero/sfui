cmake_minimum_required(VERSION 3.10)
project(UnitTests VERSION 1.0.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
# Ensure that the C++ standard is strictly followed
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable compiler-specific extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags for optimization and warnings
add_compile_options(-O2 -Wall -Wextra -Wno-unused-parameter)

# Add the internal libraries
add_subdirectory(sfui)
add_subdirectory(sfui/dependencies/CxxUtils)
add_subdirectory(sfui/dependencies/libvfs)

# Function to add test executables
function(add_test_executable test_file)
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE sfui vfs)
endfunction()

# Automatically find all test source files matching Test*.cpp pattern
file(GLOB TEST_FILES "Test*.cpp")

# Create an executable for each test file
foreach(test_file IN LISTS TEST_FILES)
    add_test_executable(${test_file})
endforeach()
