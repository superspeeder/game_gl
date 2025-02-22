//
// Created by andy on 2/18/2025.
//

// ReSharper disable CppMemberFunctionMayBeConst
#include "game/game.hpp"
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

    struct Vertex {
        glm::vec2 position;
        glm::vec2 uv;
    };

    void Game::create() {
        Vertex vertices[] = {
            {{-0.25f, -0.25f}, {0.0f, 0.0f}},
            {{0.25f, -0.25f}, {0.25f, 0.0f}},
            {{0.25f, 0.25f}, {0.25f, 0.25f}},

            {{-0.25f, -0.25f}, {0.0f, 0.0f}},
            {{0.25f, 0.25f}, {0.25f, 0.25f}},
            {{-0.25f, 0.25f}, {0.0f, 0.25f}},
        };

        Vertex screen_vertices[] = {
            {{-1.0f, -1.0f}, {0.0f, 0.0f}},
            {{1.0f, -1.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},

            {{-1.0f, -1.0f}, {0.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-1.0f, 1.0f}, {0.0f, 1.0f}},
        };


        m_VertexBuffer = std::make_shared<render::Buffer>(sizeof(vertices), vertices);
        m_VertexArray  = std::make_shared<render::VertexArray>();
        m_VertexArray->add_vertex_buffer(m_VertexBuffer.get(), {2, 2});

        m_ScreenVertexBuffer = std::make_shared<render::Buffer>(sizeof(vertices), vertices);
        m_ScreenVertexArray  = std::make_shared<render::VertexArray>();
        m_ScreenVertexArray->add_vertex_buffer(m_ScreenVertexBuffer.get(), {2, 2});

        m_Texture = render::Texture::load("assets/test.png");

        int width,height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        // build render target
        m_RenderTargetTexture = render::Texture::create_2d(width, height, render::Format::RGBA8);
        m_RenderTargetDepthStencilBuffer = std::make_shared<render::RenderBuffer>(width, height, render::Format::D24S8);
        m_RenderTarget = std::make_shared<render::Framebuffer>();
        m_RenderTarget->color_attachment(m_RenderTargetTexture.get(), 0);
        m_RenderTarget->attachment(m_RenderTargetDepthStencilBuffer.get(), render::Framebuffer::Attachment::DepthStencil);
        glTextureParameteri(m_RenderTargetTexture->get_handle(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_RenderTargetTexture2 = render::Texture::create_2d(width, height, render::Format::RGBA8);
        m_RenderTargetDepthStencilBuffer2 = std::make_shared<render::RenderBuffer>(width, height, render::Format::D24S8);
        m_RenderTarget2 = std::make_shared<render::Framebuffer>();
        m_RenderTarget2->color_attachment(m_RenderTargetTexture2.get(), 0);
        m_RenderTarget2->attachment(m_RenderTargetDepthStencilBuffer2.get(), render::Framebuffer::Attachment::DepthStencil);
        glTextureParameteri(m_RenderTargetTexture2->get_handle(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_ShaderProgram = render::ShaderProgram::load("assets/main.vert", "assets/main.frag");
        m_PostProcess = render::ShaderProgram::load("assets/post_process.vert", "assets/post_process.frag");
        m_PostProcess2 = render::ShaderProgram::load_compute("assets/post_process2.comp");
    }

    void Game::render(float delta) {
        render::clearBackground({1.0f, 0.0f, 0.0f});

        m_RenderTarget->bind();

        m_ShaderProgram->use();
        m_Texture->bind_unit(0);
        m_ShaderProgram->uniform1i("uTexture", 0);
        m_VertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        m_RenderTarget2->bind();

        m_PostProcess->use();
        m_RenderTargetTexture->bind_unit(0);
        m_PostProcess->uniform1i("uTexture", 0);
        m_PostProcess->uniform1f("uOffset", sin(m_ThisFrame / 5.0f) * 0.1f);
        m_VertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        render::Framebuffer::bind_default();
        m_PostProcess2->use();
        m_RenderTargetTexture2->bind_unit(0);
        m_PostProcess->uniform1i("uTexture", 0);
        m_VertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


} // namespace game
