#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

namespace omelet::glsl
{
class Program final
{
  public:
    struct IntegerAttribute
    {
        std::string name;
        ::gl::GLuint idx;
        ::gl::GLint size;
        ::gl::GLenum type;
        ::gl::GLintptr buffer_offset;
        ::gl::GLintptr buffer_stride;
        ::gl::GLuint relative_offset = 0;
    };

    struct FloatingPointAttribute
    {
        std::string name;
        ::gl::GLuint idx;
        ::gl::GLint size;
        ::gl::GLenum type;
        bool normalized;
        ::gl::GLintptr buffer_offset;
        ::gl::GLintptr buffer_stride;
        ::gl::GLuint relative_offset = 0;
    };

    using Attribute = std::variant<IntegerAttribute, FloatingPointAttribute>;

  private:
    struct VBO
    {
        ::gl::GLuint id{};
        ::gl::GLsizeiptr size = 0;
        std::vector<Attribute> attributes;
    };

    ::gl::GLuint m_program;
    ::gl::GLuint m_vao{};
    std::vector<VBO> m_vbos;
    ::gl::GLenum m_drawing_mode;
    bool m_moved = false;

    ::gl::GLint get_uniform_location(const std::string &name) const;

    void fill_vbo(std::size_t vbo_idx,
                  const void *data,
                  ::gl::GLsizeiptr n_bytes);

  public:
    struct Shader
    {
        ::gl::GLenum type{};
        std::string source;
    };

    [[nodiscard]] static VBO create_vbo(
        const std::vector<Attribute> &attributes)
    {
        return VBO{0, 0, attributes};
    }

    Program(const std::vector<Shader> &shaders,
            const std::vector<VBO> &vbos,
            ::gl::GLenum drawing_mode);

    Program(const Program &) = delete;

    Program(Program &&other) noexcept;

    Program &operator=(const Program &) = delete;

    Program &operator=(Program &&other) noexcept;

    ~Program();

    void set_uniform(const std::string &name, auto &&f, auto &&...args) const
    {
        f(m_program,
          get_uniform_location(name),
          std::forward<decltype(args)>(args)...);
    }

    void set_uniform(const std::string &name, float value) const;

    [[nodiscard]] std::size_t num_vbos() const;

    template<typename T>
    void fill_vbo(const std::size_t vbo_idx, std::span<T> data)
    {
        if (not data.empty()) {
            fill_vbo(vbo_idx,
                     data.data(),
                     static_cast<::gl::GLsizeiptr>(data.size() * sizeof(T)));
        }
    }

    void draw(std::size_t n_elements) const;
};
}  // namespace omelet::glsl
