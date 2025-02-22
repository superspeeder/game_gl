//
// Created by andy on 2/20/2025.
//

#include "game/render/render_target.hpp"

namespace game::render {
    RenderTarget::RenderTarget(const Description &description) {}

    RenderTarget::RenderTarget(unsigned int width, unsigned int height) {
        auto texture = std::make_unique<Texture>(Texture::Type::Texture2D);
        texture->set_image_2d(width, height, Format::RGBA8);

        auto rb = std::make_unique<RenderBuffer>(width, height, Format::D24S8);

        m_Framebuffer = std::make_unique<Framebuffer>();
        m_Framebuffer->attachment(texture.get(), Framebuffer::Attachment::Color0);
        m_Framebuffer->attachment(rb.get(), Framebuffer::Attachment::DepthStencil);

        m_Textures.insert(std::make_pair(Framebuffer::Attachment::Color0, std::move(texture)));
        m_RenderBuffers.insert(std::make_pair(Framebuffer::Attachment::DepthStencil, std::move(rb)));
    }

    const std::unique_ptr<Texture> &RenderTarget::get_texture(const Framebuffer::Attachment attachment_point) {
        const auto it = m_Textures.find(attachment_point);
        if (it == m_Textures.end()) {
            return nullptr;
        }

        return it->second;
    }

    const std::unique_ptr<RenderBuffer> &RenderTarget::get_renderbuffer(const Framebuffer::Attachment attachment_point) {
        const auto it = m_RenderBuffers.find(attachment_point);
        if (it == m_RenderBuffers.end()) {
            return nullptr;
        }

        return it->second;
    }

    void RenderTarget::bind() const {
        m_Framebuffer->bind();
    }
} // namespace game::render
