#pragma once

#include "game/render.hpp"
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

        std::shared_ptr<render::Buffer>        m_VertexBuffer;
        std::shared_ptr<render::VertexArray>   m_VertexArray;
        std::shared_ptr<render::ShaderProgram> m_ShaderProgram;

        std::shared_ptr<render::Texture> m_Texture;
    };

} // namespace game
