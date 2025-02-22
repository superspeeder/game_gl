//
// Created by andy on 2/20/2025.
//

#include "game/render/post_process_stack.hpp"

#include <stdexcept>

namespace game::render {
    PostProcessingStage::PostProcessingStage() = default;

    void PostProcessingStage::on_attached_to_stack(PostProcessingStack *stack) {
        m_Stack = stack;
    }

    PostProcessingComputeStage::PostProcessingComputeStage(const std::shared_ptr<ShaderProgram> &compute_shader) : m_ComputeProgram(compute_shader) {}

    PostProcessingStack::PostProcessingStack(const unsigned int width, const unsigned int height) : m_Width(width), m_Height(height) {

        
        m_ScreenVAO = std::make_unique<VertexArray>();
        m_ScreenVBO = std::make_unique<Buffer>(sizeof(screen_vertices), screen_vertices, Buffer::Usage::StaticDraw);

    }

    void PostProcessingStack::push_stage(const std::shared_ptr<PostProcessingStage> &stage) {
        m_Stages.push_back(stage);
        stage->on_attached_to_stack(this);
    }

    void PostProcessingStack::execute(const PostProcessingState &input_state) {
        PostProcessingState interim_state = input_state;
        for (const auto& stage : m_Stages) {
            stage->execute(interim_state);
            interim_state = { .render_target = stage->get_render_target() };
        }
    }

    void PostProcessingComputeStage::on_attached_to_stack(PostProcessingStack *stack) {
        PostProcessingStage::on_attached_to_stack(stack);

        m_RenderTarget = std::make_unique<RenderTarget>(m_Stack->get_width(), m_Stack->get_height());
    }

    RenderTarget *PostProcessingComputeStage::get_render_target() const {
        return m_RenderTarget.get();
    }

    void PostProcessingComputeStage::execute(const PostProcessingState &input_state) {
        set_uniforms();
        m_ComputeProgram->dispatch(m_Stack->get_width(), m_Stack->get_height(), 1);
    }

    void PostProcessingComputeStage::set_uniforms() const {
        m_RenderTarget->get_texture(Framebuffer::Attachment::Color0)->bind_unit(0);
        m_ComputeProgram->uniform1i("uPostProcessingSource", 0);
    }

    void PostProcessingStack::on_attached_to_stack(PostProcessingStack *stack) {
        PostProcessingStage::on_attached_to_stack(stack);
        if (m_Stack->m_Width != m_Width || m_Stack->m_Height != m_Height) {
            throw std::invalid_argument("Cannot add sub-postprocessing stack of a different width or height to a stack.");
        }
    }

    RenderTarget *PostProcessingStack::get_render_target() const {
        if (m_Stages.empty()) {
            return nullptr;
        }

        return m_Stages.back()->get_render_target();
    }


} // namespace game::render
