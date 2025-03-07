cmake_minimum_required(VERSION 3.30)
project(game_gl)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG master)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.x)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master CONFIGURE_COMMAND "" BUILD_COMMAND "")

FetchContent_MakeAvailable(glfw glm spdlog stb)

add_executable(game glad/src/gl.c
        src/main.cpp
        src/game/game.cpp
        src/game/game.hpp
        src/game/stbimpl.cpp
        src/game/render/render.cpp
        src/game/render/render.hpp
        src/game/exception.cpp
        src/game/exception.hpp
        src/game/render/post_process_stack.cpp
        src/game/render/post_process_stack.hpp
        src/game/render/render_target.cpp
        src/game/render/render_target.hpp)
target_include_directories(game PRIVATE src/ ${stb_SOURCE_DIR} glad/include/)
target_link_libraries(game PRIVATE glfw glm::glm spdlog::spdlog)

target_compile_definitions(game PRIVATE GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL)
