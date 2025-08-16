#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "glsl_program.hpp"

#include <glad/gles2.h>

#include "utilities.hpp"

namespace
{
GLuint compile_shader(const ::omelet::glsl::Program::Shader &shader)
{
    const char *source = shader.source.c_str();

    const GLuint handle = glCreateShader(shader.type);
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    GLint success;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success != static_cast<GLint>(GL_TRUE)) {
        std::string err_msg;

        GLint length = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            err_msg.resize(static_cast<std::size_t>(length));

            GLsizei written = 0;
            glGetShaderInfoLog(handle, length, &written, err_msg.data());
            err_msg.resize(static_cast<std::size_t>(written));
        }

        throw std::runtime_error("Failed to compile vertex shader: " + err_msg);
    }

    return handle;
}

GLuint link_shaders(const std::vector<::omelet::glsl::Program::Shader> &shaders)
{
    std::vector<GLuint> shader_handles;
    shader_handles.reserve(shaders.size());

    const GLuint program_handle = glCreateProgram();
    for (const auto &shader : shaders) {
        const GLuint shader_handle =
            shader_handles.emplace_back(compile_shader(shader));
        glAttachShader(program_handle, shader_handle);
    }

    glLinkProgram(program_handle);

    GLint success;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
    if (success != static_cast<GLint>(GL_TRUE)) {
        std::string err_msg;

        GLint length = 0;
        glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            err_msg.resize(static_cast<std::size_t>(length));

            GLsizei written = 0;
            glGetProgramInfoLog(
                program_handle, length, &written, err_msg.data());
            err_msg.resize(static_cast<std::size_t>(written));
        }

        throw std::runtime_error("Failed to link shader program: " + err_msg);
    }

    for (const GLuint shader_handle : shader_handles) {
        glDeleteShader(shader_handle);
    }

    return program_handle;
}

constexpr const void *make_buffer_offset(const auto offset,
                                         const auto relative_offset) noexcept
{
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<const void *>(offset + relative_offset);
}
}  // namespace

namespace omelet::glsl
{

Program::VBO Program::create_vbo(const std::vector<Attribute> &attributes,
                                 const VBOLayout layout)
{
    return VBO{.id = 0,
               .attributes = attributes,
               .offset = static_cast<GLintptr>(layout.offset),
               .stride = static_cast<GLsizei>(layout.stride),
               .size = 0};
}

Program::VBO Program::create_vbo(const Attribute &attribute,
                                 const VBOLayout layout)
{
    return create_vbo(std::vector{attribute}, layout);
}

Program::Program(const std::vector<Shader> &shaders,
                 const std::vector<VBO> &vbos,
                 const GLenum drawing_mode)
    : m_program(link_shaders(shaders))
    , m_vbos(vbos)
    , m_drawing_mode(drawing_mode)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    {
        const auto n = m_vbos.size();
        std::vector<GLuint> buffers(n);

        glGenBuffers(static_cast<GLsizei>(n), buffers.data());
        for (auto i = 0U; i < n; i++) {
            m_vbos[i].id = buffers[i];
        }
    }

    for (const auto &vbo : m_vbos) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);

        for (const auto &attrib : vbo.attributes) {
            const auto attrib_idx =
                std::visit([](const auto &attr) { return attr.idx; }, attrib);

            glEnableVertexAttribArray(attrib_idx);
            std::visit(
                overloaded{[vbo](const IntegerAttribute &attr)
                           {
                               glVertexAttribIPointer(
                                   attr.idx,
                                   attr.size,
                                   attr.type,
                                   vbo.stride,
                                   ::make_buffer_offset(vbo.offset,
                                                        attr.relative_offset));
                           },
                           [vbo](const FloatingPointAttribute &attr)
                           {
                               glVertexAttribPointer(
                                   attr.idx,
                                   attr.size,
                                   attr.type,
                                   attr.normalized ? GL_TRUE : GL_FALSE,
                                   vbo.stride,
                                   ::make_buffer_offset(vbo.offset,
                                                        attr.relative_offset));
                           }},
                attrib);
        }
    }  // namespace omelet::glsl
}

Program::Program(const std::vector<Shader> &shaders,
                 VBO &&vbo,
                 const GLenum drawing_mode)
    : Program(shaders, std::vector{std::move(vbo)}, drawing_mode)
{
}

Program::Program(Program &&other) noexcept
    : m_program(other.m_program)
    , m_vao(other.m_vao)
    , m_vbos(std::move(other.m_vbos))
    , m_drawing_mode(other.m_drawing_mode)
{
    assert(not m_moved);

    other.m_moved = true;
}

Program &Program::operator=(Program &&other) noexcept
{
    assert(not m_moved);

    m_program = other.m_program;
    m_vao = other.m_vao;
    m_vbos = std::move(other.m_vbos);
    other.m_moved = false;

    return *this;
}

Program::~Program()
{
    if (not m_moved) {
        {
            const auto n = m_vbos.size();
            std::vector<GLuint> buffers(n);
            for (auto i = 0U; i < n; i++) {
                buffers[i] = m_vbos[i].id;
            }
            glDeleteBuffers(static_cast<GLsizei>(n), buffers.data());
        }

        glDeleteVertexArrays(1, &m_vao);
        glDeleteProgram(m_program);
    }
}

GLint Program::get_uniform_location(const std::string &name)
{
    if (const auto it = m_uniform_location.find(name);
        it != m_uniform_location.end())
    {
        return it->second;
    }

    const auto idx = glGetUniformLocation(m_program, name.c_str());
    if (idx < 0) {
        throw std::runtime_error("Could not find uniform '" + name + "'");
    }

    const auto success = m_uniform_location.emplace(name, idx).second;
    assert(success);

    return idx;
}

void Program::set_uniform(const std::string &name, const float value)
{
    set_uniform(name, glUniform1f, value);
}

void Program::fill_vbo(const std::size_t vbo_idx,
                       const void *data,
                       const GLsizeiptr n_bytes)
{
    assert(vbo_idx < m_vbos.size());
    auto &vbo = m_vbos[vbo_idx];

    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    if (vbo.size < n_bytes) {
        vbo.size = n_bytes;
        glBufferData(GL_ARRAY_BUFFER, n_bytes, nullptr, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, n_bytes, data);
}

void Program::draw(const std::size_t n_elements) const
{
    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glDrawArrays(m_drawing_mode, 0, static_cast<GLsizei>(n_elements));

#ifndef NDEBUG
    if (const GLenum err = glGetError(); err != GL_NO_ERROR) {
        throw std::runtime_error("Error during drawing: "
                                 + std::to_string(err));
    }
#endif
}

}  // namespace omelet::glsl
