cmake_minimum_required(VERSION 3.10.0)
project(HexHack VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/vendor/imgui")


set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
message(STATUS "SDL2 Includes found at: ${SDL2_INCLUDE_DIRS}")
add_executable(HexHacker main.cpp ${IMGUI_SOURCES})

target_include_directories(HexHacker PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(HexHacker PRIVATE 
    ${SDL2_LIBRARIES}
    mingw32
    SDL2main
    SDL2
)