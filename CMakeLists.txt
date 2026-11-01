cmake_minimum_required(VERSION 3.20)

project(cpp_https_server
    VERSION 0.1.0
    LANGUAGES CXX
)

cmake_minimum_required(VERSION 3.20)

project(cpp_https_server
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Write build/compile_commands.json so clangd (VS Code) finds include/
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(server
    src/server.cpp
    src/FileDescriptor.cpp
)

add_executable(client
    src/client.cpp
)

foreach(target server client)
    target_include_directories(${target} PRIVATE include)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endforeach()