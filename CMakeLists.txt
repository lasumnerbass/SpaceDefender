cmake_minimum_required(VERSION 3.20)
project(SpaceDefender LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SFML_INCLUDE_DIRS})

# Source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# macOS specific settings
if(APPLE)
    # Find Homebrew SFML
    set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})
    
    # Alternative direct linking for Homebrew SFML
    target_include_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/include)
    target_link_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/lib)
    target_link_libraries(${PROJECT_NAME} 
        sfml-graphics sfml-audio sfml-window sfml-system
    )
    
    # Set minimum macOS version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
    
    # Set Xcode working directory to project root
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source files: ${SOURCES}")

# Create build script for convenience
if(UNIX)
    file(WRITE ${CMAKE_SOURCE_DIR}/build.sh 
        "#!/bin/bash\n"
        "cd \"$(dirname \"$0\")\"\n"
        "mkdir -p build\n"
        "cd build\n"
        "cmake ..\n"
        "make\n"
        "echo \"Build complete. Run ./build/SpaceDefender to play!\"\n"
    )
    execute_process(COMMAND chmod +x ${CMAKE_SOURCE_DIR}/build.sh)
endif()
