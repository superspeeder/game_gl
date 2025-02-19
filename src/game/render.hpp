//
// Created by andy on 2/18/2025.
//

#pragma once

#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include <string_view>

#include "game/exception.hpp"

namespace game::render {
    void clearBackground();
    void clearBackground(const glm::vec4 &color);

    inline void clearBackground(const glm::vec3 &color) {
        clearBackground({color, 1.0f});
    };

    class Buffer {
      public:
        enum class Target : GLenum {
            Array             = GL_ARRAY_BUFFER,
            ElementArray      = GL_ELEMENT_ARRAY_BUFFER,
            Uniform           = GL_UNIFORM_BUFFER,
            ShaderStorage     = GL_SHADER_STORAGE_BUFFER,
            AtomicCounter     = GL_ATOMIC_COUNTER_BUFFER,
            Texture           = GL_TEXTURE_BUFFER,
            Query             = GL_QUERY_BUFFER,
            TransformFeedback = GL_TRANSFORM_FEEDBACK,
            CopyRead          = GL_COPY_READ_BUFFER,
            CopyWrite         = GL_COPY_WRITE_BUFFER,
            PixelPack         = GL_PIXEL_PACK_BUFFER,
            PixelUnpack       = GL_PIXEL_UNPACK_BUFFER,
            DispatchIndirect  = GL_DISPATCH_INDIRECT_BUFFER,
            DrawIndirect      = GL_DRAW_INDIRECT_BUFFER,
        };

        enum class Usage : GLenum {
            StaticDraw = GL_STATIC_DRAW,
            StaticRead = GL_STATIC_READ,
            StaticCopy = GL_STATIC_COPY,

            DynamicDraw = GL_DYNAMIC_DRAW,
            DynamicRead = GL_DYNAMIC_READ,
            DynamicCopy = GL_DYNAMIC_COPY,

            StreamDraw = GL_STREAM_DRAW,
            StreamRead = GL_STREAM_READ,
            StreamCopy = GL_STREAM_COPY,
        };

        Buffer();
        explicit Buffer(size_t size, Usage usage = Usage::StaticDraw);
        Buffer(size_t size, const void *data, Usage usage = Usage::StaticDraw);

        ~Buffer();

        void bind(Target target) const;

        [[nodiscard]] unsigned int get_handle() const;

      private:
        unsigned int m_Buffer;
    };

    class VertexArray {
      public:
        VertexArray();
        ~VertexArray();

        void bind() const;

        void add_vertex_buffer(const Buffer *buffer, const std::vector<size_t> &attributes);
        void set_element_buffer(const Buffer *buffer);

      private:
        unsigned int m_VertexArray;
        unsigned int m_NextBinding   = 0;
        unsigned int m_NextAttribute = 0;
    };

    class ShaderModule {
      public:
        enum class Type : GLenum {
            Vertex                 = GL_VERTEX_SHADER,
            Fragment               = GL_FRAGMENT_SHADER,
            Geometry               = GL_GEOMETRY_SHADER,
            TessellationControl    = GL_TESS_CONTROL_SHADER,
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
            Compute                = GL_COMPUTE_SHADER,
        };

        ShaderModule(Type type, std::string_view text);

        ~ShaderModule();

        unsigned int get_handle() const;

      private:
        unsigned int m_ShaderModule;
        Type         m_Type;
    };

    // TODO: redo shader abstraction as a pipeline abstraction for better compatibility with trying to port this to use vulkan in the future (not important right now)
    class ShaderProgram {
      public:
        explicit ShaderProgram(const std::vector<ShaderModule *> &modules);

        static std::shared_ptr<ShaderProgram>
        create(const std::vector<ShaderModule *> &modules); // works unlike a call to std::make_shared<ShaderProgram>({module1, module2}) would.
        static std::shared_ptr<ShaderProgram> create(std::string_view vertex_source, std::string_view fragment_source);
        static std::shared_ptr<ShaderProgram> create_compute(std::string_view compute_source);
        static std::shared_ptr<ShaderProgram> create(const std::vector<std::pair<ShaderModule::Type, std::string_view>> &sources);

        static std::shared_ptr<ShaderProgram> load(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);
        static std::shared_ptr<ShaderProgram> load_compute(const std::filesystem::path &compute_path);
        static std::shared_ptr<ShaderProgram> load(const std::vector<std::pair<ShaderModule::Type, std::filesystem::path>> &paths);

        void use() const;

        int get_uniform_location(std::string_view name) const;

      private:
        unsigned int m_Program;
    };

    enum class Format {
        R8    = GL_R8,
        RG8   = GL_RG8,
        RGB8  = GL_RGB8,
        RGBA8 = GL_RGBA8,

        R32F    = GL_R32F,
        RG32F   = GL_RG32F,
        RGB32F  = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        R16    = GL_R16,
        RG16   = GL_RG16,
        RGB16  = GL_RGB16,
        RGBA16 = GL_RGBA16,

        R32I    = GL_R32I,
        RG32I   = GL_RG32I,
        RGB32I  = GL_RGB32I,
        RGBA32I = GL_RGBA32I,

        R32UI    = GL_R32UI,
        RG32UI   = GL_RG32UI,
        RGB32UI  = GL_RGB32UI,
        RGBA32UI = GL_RGBA32UI,

        R16UI    = GL_R16UI,
        RG16UI   = GL_RG16UI,
        RGB16UI  = GL_RGB16UI,
        RGBA16UI = GL_RGBA16UI,

        R16I    = GL_R16I,
        RG16I   = GL_RG16I,
        RGB16I  = GL_RGB16I,
        RGBA16I = GL_RGBA16I,

        R8I    = GL_R8I,
        RG8I   = GL_RG8I,
        RGB8I  = GL_RGB8I,
        RGBA8I = GL_RGBA8I,

        R8UI    = GL_R8UI,
        RG8UI   = GL_RG8UI,
        RGB8UI  = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,

        D24S8  = GL_DEPTH24_STENCIL8,
        D32FS8 = GL_DEPTH32F_STENCIL8,

        D32F = GL_DEPTH_COMPONENT32F,
        D24  = GL_DEPTH_COMPONENT24,
        D16  = GL_DEPTH_COMPONENT16,

        S8  = GL_STENCIL_INDEX8,
        S16 = GL_STENCIL_INDEX16,
    };

    enum class PixelType {
        I8  = GL_BYTE,
        U8  = GL_UNSIGNED_BYTE,
        I16 = GL_SHORT,
        U16 = GL_UNSIGNED_SHORT,
        I32 = GL_INT,
        U32 = GL_UNSIGNED_INT,
        F32 = GL_FLOAT,
    };

    struct ImageData {
        void        *data;
        unsigned int width, height;
        unsigned int num_components;
        PixelType    pixel_type;
        bool         preserve_int = false;

        // returned data block must be freed using stbi_image_free
        static ImageData load(const std::filesystem::path &path, unsigned int desired_num_channels = 0);
        static ImageData loadf(const std::filesystem::path &path, unsigned int desired_num_channels = 0);
    };

    class Texture {
      public:
        enum class Type : GLenum {
            Texture1D = GL_TEXTURE_1D,
            Texture2D = GL_TEXTURE_2D,
            Texture3D = GL_TEXTURE_3D,

            Texture1DArray      = GL_TEXTURE_1D_ARRAY,
            Texture2DArray      = GL_TEXTURE_2D_ARRAY,
            TextureRectangle    = GL_TEXTURE_RECTANGLE,
            TextureCubeMap      = GL_TEXTURE_CUBE_MAP,
            TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
            TextureBuffer       = GL_TEXTURE_BUFFER,

            Texture2DMS      = GL_TEXTURE_2D_MULTISAMPLE,
            Texture2DMSArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        };

        explicit Texture(Type type);

        // Both of these constructors create **owning** references to a texture. For right now, there is no way to easily form a non-owning reference to a texture from the handle.
        explicit Texture(unsigned int handle); // use Texture(unsigned int, Type) instead unless you don't know the type of the texture already.
        Texture(unsigned int handle, Type type);

        static std::vector<Texture *> create_many(Type type, std::size_t count);

        void set_image_2d(const ImageData &image_data);

        void bind() const;
        void bind_unit(unsigned int unit) const;

        static std::shared_ptr<Texture> load(const std::filesystem::path &path);

        [[nodiscard]] unsigned int get_handle() const noexcept;

      private:
        Type         m_Type;
        unsigned int m_Texture;
    };
} // namespace game::render
