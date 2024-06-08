# use vcpkg to download and build dependencies
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME cylindrix)
project(${PROJECT_NAME} LANGUAGES CXX)

# opengl
find_package(OpenGL REQUIRED)
include_directories(${GL_INCLUDE_DIRS})

# sdl2
find_package(SDL2 CONFIG REQUIRED)

# glew
find_package(GLEW REQUIRED)

# glm
find_package(glm CONFIG REQUIRED)

# openal-soft
find_package(OpenAL CONFIG REQUIRED)

# lib123
find_package(mpg123 CONFIG REQUIRED)

# libyaml
find_package(yaml CONFIG REQUIRED)

add_compile_definitions(GLCYLINDRIX)

# src
include_directories(src)
add_executable(${PROJECT_NAME}
    src/ai.cpp
    src/ai_move.cpp
    src/ai_util.cpp
    src/base.cpp
    src/clipping.cpp
    src/collide.cpp
    src/commands.cpp
	src/config.cpp
    src/do_state.cpp
    src/energy.cpp
    src/events.cpp
    src/explode.cpp
    src/fli.cpp
    src/fx.cpp
    src/gameloop.cpp
    src/glescylindrix.cpp
	src/hud.cpp
    src/jonsb.cpp
    src/keys.cpp
    src/level.cpp
    src/main.cpp
    src/menu.cpp
    src/motor.cpp
	src/movement.cpp
    src/object.cpp
    src/omega.cpp
    src/path.cpp
    src/pcx.cpp
    src/prim.cpp
    src/project.cpp
    src/pylon.cpp
	src/radar.cpp
    src/states.cpp
    src/stats.cpp
    src/stub.cpp
    src/tanks.cpp
    src/text.cpp
	src/util.cpp
    src/voices.cpp
    src/system/sdl/system.cpp
    src/system/openal/sound.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    GLEW::GLEW
    glm::glm
    yaml
    OpenAL::OpenAL
    MPG123::libmpg123
)

if(SHIPPING)
    add_compile_definitions(SHIPPING)

    # Copy required data directories to the executable directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/font
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/font
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/shader
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/texture
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/texture
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/data/test.ply
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/data/test.ply
    )
endif()