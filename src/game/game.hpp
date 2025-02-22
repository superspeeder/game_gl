#pragma once

#include "game/render/render.hpp"
#include <GLFW/glfw3.h>

#include <memory>

namespace game {
    struct InitWrapper {
        InitWrapper();
        virtual ~InitWrapper();
    };

    class Game final : public InitWrapper {
      public:
        Game();
        ~Game() override;

        Game(const Game &)            = delete;
        Game &operator=(const Game &) = delete;

        void mainloop();

        void create();
        void render(float delta);

      private:
        GLFWwindow *m_Window;

        float m_DeltaTime;
        float m_LastFrame;
        float m_ThisFrame;

        std::shared_ptr<render::Buffer>      m_VertexBuffer;
        std::shared_ptr<render::VertexArray> m_VertexArray;

        std::shared_ptr<render::Buffer>      m_ScreenVertexBuffer;
        std::shared_ptr<render::VertexArray> m_ScreenVertexArray;

        std::shared_ptr<render::ShaderProgram> m_ShaderProgram;

        std::shared_ptr<render::Texture> m_Texture;

        std::shared_ptr<render::Texture>      m_RenderTargetTexture;
        std::shared_ptr<render::RenderBuffer> m_RenderTargetDepthStencilBuffer;
        std::shared_ptr<render::Framebuffer>  m_RenderTarget;

        std::shared_ptr<render::Texture>      m_RenderTargetTexture2;
        std::shared_ptr<render::RenderBuffer> m_RenderTargetDepthStencilBuffer2;
        std::shared_ptr<render::Framebuffer>  m_RenderTarget2;

        std::shared_ptr<render::ShaderProgram> m_PostProcess;
        std::shared_ptr<render::ShaderProgram> m_PostProcess2;
    };

} // namespace game
