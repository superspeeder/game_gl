//
// Created by andy on 2/18/2025.
//

#include "game.hpp"
#include <glad/gl.h>

namespace game {
    InitWrapper::InitWrapper() {
        glfwInit();
    }

    InitWrapper::~InitWrapper() {
        glfwTerminate();
    }

    Game::Game() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_DEBUG, true);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
        glfwWindowHint(GLFW_RESIZABLE, false);
        glfwWindowHint(GLFW_SAMPLES, 8);

        m_Window = glfwCreateWindow(640, 480, "Game", nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);
        gladLoadGL(glfwGetProcAddress);
    }

    Game::~Game() {}

    void Game::mainloop() {
        create();

        m_DeltaTime = 1.0f / 60.0f;
        m_ThisFrame = glfwGetTime();
        m_LastFrame = m_ThisFrame - m_DeltaTime;

        while (!glfwWindowShouldClose(m_Window)) {
            glfwPollEvents();

            render(m_DeltaTime);

            glfwSwapBuffers(m_Window);

            m_LastFrame = m_ThisFrame;
            m_ThisFrame = glfwGetTime();
            m_DeltaTime = m_ThisFrame - m_LastFrame;
        }
    }

    void Game::create() {
        float vertices[] = {
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };

        m_VertexBuffer = std::make_shared<render::Buffer>(sizeof(vertices), vertices);
        m_VertexArray = std::make_shared<render::VertexArray>();
        m_VertexArray->add_vertex_buffer(m_VertexBuffer.get(), {2, 3});

        m_ShaderProgram = render::ShaderProgram::create(
            R"(#version 460 core
layout(location = 0) in vec2 posIn;
layout(location = 1) in vec3 colorIn;

out vec3 fColor;

void main() {
    gl_Position = vec4(posIn, 0.0, 1.0); // should error
    fColor = colorIn;
})",
            R"(#version 460 core
in vec3 fColor;

out vec4 colorOut;

void main() {
    colorOut = vec4(fColor, 1.0);
}
)");
    }

    void Game::render(float delta) {
        render::clearBackground({1.0f, 0.0f, 1.0f});

        m_ShaderProgram->use();
        m_VertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }


} // namespace game
