//
// Created by andy on 2/20/2025.
//

#pragma once

#include "game/render/render.hpp"

namespace game::render {

    class RenderTarget {
    public:
        struct Attachment {
            Framebuffer::Attachment attachment_point;
            Format format;
            bool renderbuffer = false;
        };

        struct Description {
            unsigned int width, height;
            std::vector<Attachment> attachments;
        };

        explicit RenderTarget(const Description& description);
        RenderTarget(unsigned int width, unsigned int height);

        const std::unique_ptr<Texture>& get_texture(Framebuffer::Attachment attachment_point);
        const std::unique_ptr<RenderBuffer>& get_renderbuffer(Framebuffer::Attachment attachment_point);

        void bind() const;
    private:
        std::unique_ptr<Framebuffer> m_Framebuffer;
        std::unordered_map<Framebuffer::Attachment, std::unique_ptr<Texture>> m_Textures;
        std::unordered_map<Framebuffer::Attachment, std::unique_ptr<RenderBuffer>> m_RenderBuffers;

    };

} // namespace game::render
