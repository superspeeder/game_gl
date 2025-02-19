//
// Created by andy on 2/18/2025.
//

#include "game/render.hpp"

#include <format>
#include <fstream>
#include <stb_image.h>
#include <string>

namespace game::render {

    void clearBackground() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void clearBackground(const glm::vec4 &color) {
        glClearColor(color.r, color.g, color.b, color.a);
        clearBackground();
    }

    Buffer::Buffer() {
        glCreateBuffers(1, &m_Buffer);
    }

    Buffer::Buffer(const size_t size, const Usage usage) {
        glCreateBuffers(1, &m_Buffer);
        glNamedBufferData(m_Buffer, size, nullptr, static_cast<GLenum>(usage));
    }

    Buffer::Buffer(const size_t size, const void *const data, const Usage usage) {
        glCreateBuffers(1, &m_Buffer);
        glNamedBufferData(m_Buffer, size, data, static_cast<GLenum>(usage));
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &m_Buffer);
    }

    void Buffer::bind(Target target) const {
        glBindBuffer(static_cast<GLenum>(target), m_Buffer);
    }

    unsigned int Buffer::get_handle() const {
        return m_Buffer;
    }

    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &m_VertexArray);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &m_VertexArray);
    }

    void VertexArray::bind() const {
        glBindVertexArray(m_VertexArray);
    }

    void VertexArray::add_vertex_buffer(const Buffer *const buffer, const std::vector<size_t> &attributes) {
        GLsizei stride = 0;

        for (const auto &attribute : attributes) {
            glVertexArrayAttribBinding(m_VertexArray, m_NextAttribute, m_NextBinding);
            glVertexArrayAttribFormat(m_VertexArray, m_NextAttribute, static_cast<GLint>(attribute), GL_FLOAT, false, stride);
            stride += sizeof(GLfloat) * attribute;
            glEnableVertexArrayAttrib(m_VertexArray, m_NextAttribute++);
        }

        glVertexArrayVertexBuffer(m_VertexArray, m_NextBinding++, buffer->get_handle(), 0, stride);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void VertexArray::set_element_buffer(const Buffer *const buffer) {
        glVertexArrayElementBuffer(m_VertexArray, buffer->get_handle());
    }

    ShaderModule::ShaderModule(Type type, const std::string_view text) : m_Type(type) {
        m_ShaderModule  = glCreateShader(static_cast<GLenum>(type));
        const char *src = text.data();

        // TODO: shader preprocessing stage

        glShaderSource(m_ShaderModule, 1, &src, nullptr);
        glCompileShader(m_ShaderModule);

        int status;
        glGetShaderiv(m_ShaderModule, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            int length;
            glGetShaderiv(m_ShaderModule, GL_INFO_LOG_LENGTH, &length);
            std::string info_log(length, '\0');
            glGetShaderInfoLog(m_ShaderModule, length, &length, &info_log[0]);
            throw shader_compile_error(info_log);
        }
    }

    ShaderModule::~ShaderModule() {
        glDeleteShader(m_ShaderModule);
    }

    unsigned int ShaderModule::get_handle() const {
        return m_ShaderModule;
    }

    ShaderProgram::ShaderProgram(const std::vector<ShaderModule *> &modules) {
        m_Program = glCreateProgram();
        for (const auto &module : modules) {
            glAttachShader(m_Program, module->get_handle());
        }
        glLinkProgram(m_Program);
        int status;
        glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            int length;
            glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &length);
            std::string info_log(length, '\0');
            glGetProgramInfoLog(m_Program, length, &length, &info_log[0]);
            throw shader_link_error(info_log);
        }
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::create(const std::vector<ShaderModule *> &modules) {
        return std::make_shared<ShaderProgram>(modules);
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::create(std::string_view vertex_source, std::string_view fragment_source) {
        return create({{ShaderModule::Type::Vertex, vertex_source}, {ShaderModule::Type::Fragment, fragment_source}});
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::create_compute(std::string_view compute_source) {
        return create({{ShaderModule::Type::Compute, compute_source}});
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::create(const std::vector<std::pair<ShaderModule::Type, std::string_view>> &sources) {
        std::vector<ShaderModule *> modules;
        modules.reserve(sources.size());
        for (const auto &[type, source] : sources) {
            modules.emplace_back(new ShaderModule(type, source));
        }

        auto prog = std::make_shared<ShaderProgram>(modules);

        for (const auto *module : modules) {
            delete module;
        }

        return prog;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::load(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path) {
        return load({{ShaderModule::Type::Vertex, vertex_path}, {ShaderModule::Type::Fragment, fragment_path}});
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::load_compute(const std::filesystem::path &compute_path) {
        return load({{ShaderModule::Type::Compute, compute_path}});
    }

    namespace {
        std::string read_file(const std::filesystem::path &path) {
            std::ifstream        f(path, std::ios::in | std::ios::ate);
            const std::streampos end = f.tellg();
            f.seekg(0, std::ios::beg);
            std::string text(end, '\0');
            f.read(text.data(), end);
            return text;
        }
    } // namespace

    std::shared_ptr<ShaderProgram> ShaderProgram::load(const std::vector<std::pair<ShaderModule::Type, std::filesystem::path>> &paths) {
        std::vector<ShaderModule *> modules;
        modules.reserve(paths.size());
        for (const auto &[type, path] : paths) {
            std::string text = read_file(path);
            modules.emplace_back(new ShaderModule(type, text));
        }

        auto prog = std::make_shared<ShaderProgram>(modules);

        for (const auto *module : modules) {
            delete module;
        }

        return prog;
    }

    void ShaderProgram::use() const {
        glUseProgram(m_Program);
    }

    ImageData ImageData::load(const std::filesystem::path &path, const unsigned int desired_num_channels) {
        ImageData data{};
        data.pixel_type = PixelType::U8;

        stbi_set_flip_vertically_on_load_thread(true);
        int w, h, nc;
        data.data           = stbi_load(path.string().c_str(), &w, &h, &nc, desired_num_channels);
        data.width          = w;
        data.height         = h;
        data.num_components = nc;
        return data;
    }

    ImageData ImageData::loadf(const std::filesystem::path &path, const unsigned int desired_num_channels) {
        ImageData data{};
        data.pixel_type = PixelType::F32;

        stbi_set_flip_vertically_on_load_thread(true);
        int w, h, nc;
        data.data           = stbi_loadf(path.string().c_str(), &w, &h, &nc, desired_num_channels);
        data.width          = w;
        data.height         = h;
        data.num_components = nc;
        return data;
    }

    Texture::Texture(Type type) : m_Type(type) {
        glCreateTextures(static_cast<GLenum>(type), 1, &m_Texture);
    }

    Texture::Texture(const unsigned int handle) : m_Texture(handle) {
        glGetTextureParameteriv(m_Texture, GL_TEXTURE_TARGET, reinterpret_cast<GLint *>(&m_Type)); // magic (not really, just querying the texture about what it is
    }

    Texture::Texture(const unsigned int handle, const Type type) : m_Type(type), m_Texture(handle) {}

    std::vector<Texture *> Texture::create_many(const Type type, const std::size_t count) {
        std::vector<unsigned int> handles(count, 0);
        std::vector<Texture *>    textures(count, nullptr);
        glCreateTextures(static_cast<GLenum>(type), count, handles.data());
        for (std::size_t i = 0; i < count; i++) {
            textures[i] = new Texture(handles[i], type);
        }

        return textures;
    }

    namespace {
        GLint ifmt_r(const PixelType pt, const bool preserve_int) {
            switch (pt) {
            case PixelType::I8:
                return preserve_int ? GL_R8I : GL_R8;
            case PixelType::U8:
                return preserve_int ? GL_R8UI : GL_R8;
            case PixelType::I16:
                return preserve_int ? GL_R16I : GL_R16;
            case PixelType::U16:
                return preserve_int ? GL_R16UI : GL_R16;
            case PixelType::I32:
                return GL_R32I;
            case PixelType::U32:
                return GL_R32UI;
            case PixelType::F32:
                return GL_R32F;
            default:
                throw std::invalid_argument("Invalid pixel type");
            }
        }

        GLint ifmt_rg(const PixelType pt, const bool preserve_int) {
            switch (pt) {
            case PixelType::I8:
                return preserve_int ? GL_RG8I : GL_RG8;
            case PixelType::U8:
                return preserve_int ? GL_RG8UI : GL_RG8;
            case PixelType::I16:
                return preserve_int ? GL_RG16I : GL_RG16;
            case PixelType::U16:
                return preserve_int ? GL_RG16UI : GL_RG16;
            case PixelType::I32:
                return GL_RG32I;
            case PixelType::U32:
                return GL_RG32UI;
            case PixelType::F32:
                return GL_RG32F;
            default:
                throw std::invalid_argument("Invalid pixel type");
            }
        }

        GLint ifmt_rgb(const PixelType pt, const bool preserve_int) {
            switch (pt) {
            case PixelType::I8:
                return preserve_int ? GL_RGB8I : GL_RGB8;
            case PixelType::U8:
                return preserve_int ? GL_RGB8UI : GL_RGB8;
            case PixelType::I16:
                return preserve_int ? GL_RGB16I : GL_RGB16;
            case PixelType::U16:
                return preserve_int ? GL_RGB16UI : GL_RGB16;
            case PixelType::I32:
                return GL_RGB32I;
            case PixelType::U32:
                return GL_RGB32UI;
            case PixelType::F32:
                return GL_RGB32F;
            default:
                throw std::invalid_argument("Invalid pixel type");
            }
        }

        GLint ifmt_rgba(const PixelType pt, const bool preserve_int) {
            switch (pt) {
            case PixelType::I8:
                return preserve_int ? GL_RGBA8I : GL_RGBA8;
            case PixelType::U8:
                return preserve_int ? GL_RGBA8UI : GL_RGBA8;
            case PixelType::I16:
                return preserve_int ? GL_RGBA16I : GL_RGBA16;
            case PixelType::U16:
                return preserve_int ? GL_RGBA16UI : GL_RGBA16;
            case PixelType::I32:
                return GL_RGBA32I;
            case PixelType::U32:
                return GL_RGBA32UI;
            case PixelType::F32:
                return GL_RGBA32F;
            default:
                throw std::invalid_argument("Invalid pixel type");
            }
        }
    } // namespace

    void Texture::set_image_2d(const ImageData &image_data) {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        GLint  ifmt;
        GLenum fmt;
        switch (image_data.num_components) {
        case 1:
            fmt  = GL_RED;
            ifmt = ifmt_r(image_data.pixel_type, image_data.preserve_int);
            break;
        case 2:
            fmt  = GL_RG;
            ifmt = ifmt_rg(image_data.pixel_type, image_data.preserve_int);
            break;
        case 3:
            fmt  = GL_RGB;
            ifmt = ifmt_rgb(image_data.pixel_type, image_data.preserve_int);
            break;
        case 4:
            fmt  = GL_RGBA;
            ifmt = ifmt_rgba(image_data.pixel_type, image_data.preserve_int);
            break;
        default:
            throw std::invalid_argument("Bad image data (invalid number of channels).");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, ifmt, image_data.width, image_data.height, 0, fmt, static_cast<GLenum>(image_data.pixel_type), image_data.data);
    }

    void Texture::bind() const {
        glBindTexture(static_cast<GLenum>(m_Type), m_Texture);
    }

    void Texture::bind_unit(const unsigned int unit) const {
        // TODO: some kind of fancy assert here that bases on some globally collected limit info (i.e. a kind of assert macro formed like: assert_below_limit(MAX_COMBINED_TEXTURE_IMAGE_UNITS, unit);

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(static_cast<GLenum>(unit), m_Texture);
    }
} // namespace game::render
