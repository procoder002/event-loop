cmake_minimum_required(VERSION 2.8)

project(evtloop)

# Compiler and flags
set(CMAKE_CXX_STANDARD 11)  # Or a more recent standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g") # Add -Wall and -Wextra

# Include directory
include_directories(./include)

# Source files
set(SRCS
    src/event_loop.cpp
    src/exec_ctx.cpp
    src/exec_env.cpp
    src/file_fd.cpp
    src/timer.cpp
)

# Static library name
set(LIB_NAME evtloop)  # Just the name, CMake adds the prefix and extension

# Output binary (if you want to create an executable as well)
set(BINARY event_loop)

# Create the static library
add_library(${LIB_NAME} STATIC ${SRCS})

# Create the executable (optional)
add_executable(${BINARY} src/app.cpp) # Assuming you have a main.cpp file
target_link_libraries(${BINARY} ${LIB_NAME}) # Link the executable with the library
