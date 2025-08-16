#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <glad/gles2.h>

namespace omelet::glsl
{
class Program final
{
  public:
    struct IntegerAttribute
    {
        GLuint idx;
        GLint size;
        GLenum type;
        GLuint relative_offset;
    };

    struct FloatingPointAttribute
    {
        GLuint idx;
        GLint size;
        GLenum type;
        bool normalized;
        GLuint relative_offset;
    };

    using Attribute = std::variant<IntegerAttribute, FloatingPointAttribute>;

  private:
    struct VBO
    {
        GLuint id{};
        std::vector<Attribute> attributes;
        GLintptr offset;
        GLsizei stride;
        GLsizeiptr size = 0;
    };

    GLuint m_program;
    GLuint m_vao{};
    std::vector<VBO> m_vbos;
    GLenum m_drawing_mode;
    std::unordered_map<std::string, GLint> m_uniform_location;
    bool m_moved = false;

    GLint get_uniform_location(const std::string &name);

    void fill_vbo(std::size_t vbo_idx, const void *data, GLsizeiptr n_bytes);

  public:
    struct Shader
    {
        GLenum type{};
        std::string source;
    };

    struct VBOLayout
    {
        std::size_t stride;
        std::size_t offset;
    };
    [[nodiscard]] static VBO create_vbo(const Attribute &attribute,
                                        VBOLayout layout);

    [[nodiscard]] static VBO create_vbo(
        const std::vector<Attribute> &attributes, VBOLayout layout);

    Program(const std::vector<Shader> &shaders,
            const std::vector<VBO> &vbos,
            GLenum drawing_mode);

    Program(const std::vector<Shader> &shaders, VBO &&vbo, GLenum drawing_mode);

    Program(const Program &) = delete;

    Program(Program &&other) noexcept;

    Program &operator=(const Program &) = delete;

    Program &operator=(Program &&other) noexcept;

    ~Program();

    void set_uniform(const std::string &name, auto &&f, auto &&...args)
    {
        glUseProgram(m_program);
        f(get_uniform_location(name), std::forward<decltype(args)>(args)...);
    }

    void set_uniform(const std::string &name, float value);

    template<typename T>
    void fill_vbo(const std::size_t vbo_idx, std::span<T> data)
    {
        if (not data.empty()) {
            fill_vbo(vbo_idx,
                     data.data(),
                     static_cast<GLsizeiptr>(data.size() * sizeof(T)));
        }
    }

    void draw(std::size_t n_elements) const;
};
}  // namespace omelet::glsl
