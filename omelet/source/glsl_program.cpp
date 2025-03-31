#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "glsl_program.hpp"

#include <glbinding/gl/boolean.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/types.h>

#include "utilities.hpp"

namespace
{
::gl::GLuint compile_shader(const ::omelet::glsl::Program::Shader &shader)
{
    const char *source = shader.source.c_str();

    const ::gl::GLuint handle = ::gl::glCreateShader(shader.type);
    ::gl::glShaderSource(handle, 1, &source, nullptr);
    ::gl::glCompileShader(handle);

    ::gl::GLint success;
    ::gl::glGetShaderiv(handle, ::gl::GL_COMPILE_STATUS, &success);
    if (success != static_cast<::gl::GLint>(::gl::GL_TRUE)) {
        std::string err_msg;

        ::gl::GLint length = 0;
        ::gl::glGetShaderiv(handle, ::gl::GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            err_msg.resize(static_cast<std::size_t>(length));

            ::gl::GLsizei written = 0;
            ::gl::glGetShaderInfoLog(handle, length, &written, err_msg.data());
            err_msg.resize(static_cast<std::size_t>(written));
        }

        throw std::runtime_error("Failed to compile vertex shader:" + err_msg);
    }

    return handle;
}

::gl::GLuint link_shaders(
    const std::vector<::omelet::glsl::Program::Shader> &shaders)
{
    std::vector<::gl::GLuint> shader_handles;
    shader_handles.reserve(shaders.size());

    const ::gl::GLuint program_handle = ::gl::glCreateProgram();
    for (const auto &shader : shaders) {
        const ::gl::GLuint shader_handle =
            shader_handles.emplace_back(compile_shader(shader));
        ::gl::glAttachShader(program_handle, shader_handle);
    }

    ::gl::glLinkProgram(program_handle);

    ::gl::GLint success;
    ::gl::glGetProgramiv(program_handle, ::gl::GL_LINK_STATUS, &success);
    if (success != static_cast<::gl::GLint>(::gl::GL_TRUE)) {
        std::string err_msg;

        ::gl::GLint length = 0;
        ::gl::glGetProgramiv(program_handle, ::gl::GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            err_msg.resize(static_cast<std::size_t>(length));

            ::gl::GLsizei written = 0;
            ::gl::glGetProgramInfoLog(
                program_handle, length, &written, err_msg.data());
            err_msg.resize(static_cast<std::size_t>(written));
        }

        throw std::runtime_error("Failed to link shader program:" + err_msg);
    }

    for (const ::gl::GLuint shader_handle : shader_handles) {
        ::gl::glDeleteShader(shader_handle);
    }

    return program_handle;
}
}  // namespace

namespace omelet::glsl
{
Program::Program(const std::vector<Shader> &shaders,
                 const std::vector<VBO> &vbos,
                 const ::gl::GLenum drawing_mode)
    : m_program(link_shaders(shaders))
    , m_vbos(vbos)
    , m_drawing_mode(drawing_mode)
{
    ::gl::glGenVertexArrays(1, &m_vao);
    ::gl::glBindVertexArray(m_vao);

    {
        const auto n = m_vbos.size();
        std::vector<::gl::GLuint> buffers(n);
        ::gl::glCreateBuffers(static_cast<::gl::GLsizei>(n), buffers.data());
        for (auto i = 0U; i < n; i++) {
            m_vbos[i].id = buffers[i];
        }
    }

    ::gl::GLuint binding_idx = 0;
    for (const auto &vbo : m_vbos) {
        for (const auto &attrib : vbo.attributes) {
            ::gl::glEnableVertexAttribArray(binding_idx);

            const auto [attrib_idx, buffer_offset, buffer_stride] = std::visit(
                [](const auto &attr) {
                    return std::make_tuple(
                        attr.idx, attr.buffer_offset, attr.buffer_stride);
                },
                attrib);

            ::gl::glVertexArrayAttribBinding(m_vao, attrib_idx, binding_idx);

            ::gl::glVertexArrayVertexBuffer(
                m_vao,
                binding_idx,
                vbo.id,
                buffer_offset,
                static_cast<::gl::GLsizei>(buffer_stride));

            std::visit(
                overloaded{
                    [vao = m_vao](const IntegerAttribute &attr)
                    {
                        ::gl::glVertexArrayAttribIFormat(vao,
                                                         attr.idx,
                                                         attr.size,
                                                         attr.type,
                                                         attr.relative_offset);
                    },
                    [vao = m_vao](const FloatingPointAttribute &attr)
                    {
                        ::gl::glVertexArrayAttribFormat(
                            vao,
                            attr.idx,
                            attr.size,
                            attr.type,
                            attr.normalized ? ::gl::GL_TRUE : ::gl::GL_FALSE,
                            attr.relative_offset);
                    }},
                attrib);
        }

        binding_idx += 1;
    }
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
            std::vector<::gl::GLuint> buffers(n);
            for (auto i = 0U; i < n; i++) {
                buffers[i] = m_vbos[i].id;
            }
            ::gl::glDeleteBuffers(static_cast<::gl::GLsizei>(n),
                                  buffers.data());
        }

        ::gl::glDeleteVertexArrays(1, &m_vao);
        ::gl::glDeleteProgram(m_program);
    }
}

::gl::GLint Program::get_uniform_location(const std::string &name)
{
    if (const auto it = m_uniform_location.find(name);
        it != m_uniform_location.end())
    {
        return it->second;
    }

    const auto idx = ::gl::glGetUniformLocation(m_program, name.c_str());
    if (idx < 0) {
        throw std::runtime_error("Could not find uniform " + name);
    }

    const auto success = m_uniform_location.emplace(name, idx).second;
    assert(success);

    return idx;
}

void Program::set_uniform(const std::string &name, const float value)
{
    set_uniform(name, ::gl::glProgramUniform1f, value);
}

void Program::fill_vbo(const std::size_t vbo_idx,
                       const void *data,
                       const ::gl::GLsizeiptr n_bytes)
{
    assert(vbo_idx < m_vbos.size());
    auto &vbo = m_vbos[vbo_idx];

    if (vbo.size < n_bytes) {
        vbo.size = n_bytes;
        ::gl::glNamedBufferData(
            vbo.id, n_bytes, nullptr, ::gl::GL_DYNAMIC_DRAW);
    }
    ::gl::glNamedBufferSubData(vbo.id, 0, n_bytes, data);
}

void Program::draw(const std::size_t n_elements) const
{
    ::gl::glUseProgram(m_program);
    ::gl::glBindVertexArray(m_vao);
    ::gl::glDrawArrays(
        m_drawing_mode, 0, static_cast<::gl::GLsizei>(n_elements));

    if (const ::gl::GLenum err = ::gl::glGetError(); err != ::gl::GL_NO_ERROR) {
        throw std::runtime_error(
            "Error during drawing: "
            + std::to_string(
                static_cast<std::underlying_type_t<::gl::GLenum>>(err)));
    }
}

}  // namespace omelet::glsl
