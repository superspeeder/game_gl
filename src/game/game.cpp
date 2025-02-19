//
// Created by andy on 2/18/2025.
//

#include "game.hpp"
#include <glad/gl.h>

#include <iostream>

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

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(
            +[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
                std::cerr << "OpenGL Message [" << id << "]: " << message << std::endl;
            },
            nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
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
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        };

        m_VertexBuffer = std::make_shared<render::Buffer>(sizeof(vertices), vertices);
        m_VertexArray  = std::make_shared<render::VertexArray>();
        m_VertexArray->add_vertex_buffer(m_VertexBuffer.get(), {2, 2});

        m_ShaderProgram = render::ShaderProgram::create(
            R"(#version 460 core
layout(location = 0) in vec2 posIn;
layout(location = 1) in vec2 uvIn;

out vec2 fUV;

void main() {
    gl_Position = vec4(posIn, 0.0, 1.0); // should error
    fUV = uvIn;
})",
            R"(#version 460 core
in vec2 fUV;

out vec4 colorOut;

uniform sampler2D uTexture;

void main() {
    colorOut = texture(uTexture, fUV);
}
)");

        m_Texture = render::Texture::load("assets/test.png");

        m_Texture->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    }

    void Game::render(float delta) {
        render::clearBackground({1.0f, 0.0f, 0.0f});

        m_ShaderProgram->use();
        m_Texture->bind_unit(0);
        int loc = m_ShaderProgram->get_uniform_location("uTexture");
        glUniform1i(loc, 0);
        m_VertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


} // namespace game
