//
// Created by andy on 2/20/2025.
//

#pragma once

#include "game/render/render.hpp"
#include "render_target.hpp"
#include <vector>

namespace game::render {

    struct PostProcessingState {
        RenderTarget *render_target;
    };

    class PostProcessingStack;

    class PostProcessingStage {
      public:
        PostProcessingStage();
        virtual ~PostProcessingStage() = default;

        virtual void on_attached_to_stack(PostProcessingStack *stack);

        virtual void          execute(const PostProcessingState &input_state) = 0;
        virtual RenderTarget *get_render_target() const                       = 0;

      protected:
        PostProcessingStack *m_Stack;
    };

    class PostProcessingComputeStage : public PostProcessingStage {
      public:
        explicit PostProcessingComputeStage(const std::shared_ptr<ShaderProgram> &compute_shader);
        ~PostProcessingComputeStage() override = default;

        void on_attached_to_stack(PostProcessingStack *stack) override;

        RenderTarget *get_render_target() const override;

        void execute(const PostProcessingState &input_state) override;

        virtual void set_uniforms() const;

        [[nodiscard]] inline const std::shared_ptr<ShaderProgram> &get_shader() const { return m_ComputeProgram; };

      private:
        std::shared_ptr<ShaderProgram> m_ComputeProgram;
        std::unique_ptr<RenderTarget>  m_RenderTarget;
    };

    class PostProcessingRenderStage : public PostProcessingStage {
    public:
        explicit PostProcessingRenderStage(const std::shared_ptr<ShaderProgram> &graphics_program);
        ~PostProcessingRenderStage() override = default;

        void on_attached_to_stack(PostProcessingStack *stack) override;

        RenderTarget *get_render_target() const override;

        void execute(const PostProcessingState &input_state) override;

        virtual void set_uniforms() const;

        [[nodiscard]] inline const std::shared_ptr<ShaderProgram> &get_shader() const { return m_GraphicsProgram; };

    private:
        std::shared_ptr<ShaderProgram> m_GraphicsProgram;
        std::unique_ptr<RenderTarget>  m_RenderTarget;
    };

    class PostProcessingStack final : public PostProcessingStage {
      public:
        PostProcessingStack(unsigned int width, unsigned int height);

        void push_stage(const std::shared_ptr<PostProcessingStage> &stage);

        void execute(const PostProcessingState &input_state) override;

        void on_attached_to_stack(PostProcessingStack *stack) override;

        [[nodiscard]] unsigned int get_width() const { return m_Width; }

        [[nodiscard]] unsigned int get_height() const { return m_Height; }

        RenderTarget *get_render_target() const override;

      private:
        std::vector<std::shared_ptr<PostProcessingStage>> m_Stages;

        std::unique_ptr<VertexArray> m_ScreenVAO;
        std::unique_ptr<Buffer> m_ScreenVBO;

        unsigned int m_Width, m_Height;
    };

} // namespace game::render
