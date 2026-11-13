#include "platform/pc/PcOverlay.hpp"

#include <stddef.h>

#include "platform/pc/GlApi.hpp"
#include "platform/pc/Log.hpp"

namespace
{
const Logger sLog("Overlay");

const char* const kVertexSource = R"(#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColour;
uniform vec2 uScreen;
uniform float uFlip;
out vec2 vTexCoord;
out vec4 vColour;
void main()
{
    vec2 clip = aPosition / uScreen * 2.0 - 1.0;
    gl_Position = vec4(clip.x, clip.y * uFlip, 0.0, 1.0);
    vTexCoord = aTexCoord;
    vColour = aColour;
}
)";

GLuint CompileShader(GLenum type, const char* source, const char* name)
{
    GLuint shader = Gl::CreateShader(type);
    Gl::ShaderSource(shader, 1, &source, NULL);
    Gl::CompileShader(shader);
    GLint status = 0;
    Gl::GetShaderiv(shader, Gl::COMPILE_STATUS, &status);
    if (status == 0)
    {
        char log[1024];
        GLsizei length = 0;
        Gl::GetShaderInfoLog(shader, sizeof(log), &length, log);
        LOG_ERROR(sLog, "%s shader failed: %.*s", name, (int)length, log);
        Gl::DeleteShader(shader);
        return 0;
    }
    return shader;
}
}

bool PcOverlayInitialise(PcOverlayState* state, const char* fragmentSource, const char* samplerName,
                         const char* name)
{
    state->initialised = true;
    GLuint vertex = CompileShader(Gl::VERTEX_SHADER, kVertexSource, name);
    GLuint fragment = CompileShader(Gl::FRAGMENT_SHADER, fragmentSource, name);
    if (vertex == 0 || fragment == 0)
    {
        if (vertex != 0)
            Gl::DeleteShader(vertex);
        if (fragment != 0)
            Gl::DeleteShader(fragment);
        return false;
    }

    state->program = Gl::CreateProgram();
    Gl::AttachShader(state->program, vertex);
    Gl::AttachShader(state->program, fragment);
    Gl::LinkProgram(state->program);
    Gl::DeleteShader(vertex);
    Gl::DeleteShader(fragment);

    GLint status = 0;
    Gl::GetProgramiv(state->program, Gl::LINK_STATUS, &status);
    if (status == 0)
    {
        LOG_ERROR(sLog, "%s program failed to link", name);
        Gl::DeleteProgram(state->program);
        state->program = 0;
        return false;
    }

    state->screenLocation = Gl::GetUniformLocation(state->program, "uScreen");
    state->flipLocation = Gl::GetUniformLocation(state->program, "uFlip");
    state->samplerLocation = Gl::GetUniformLocation(state->program, samplerName);

    Gl::GenVertexArrays(1, &state->vertexArray);
    Gl::GenBuffers(1, &state->vertexBuffer);
    Gl::BindVertexArray(state->vertexArray);
    Gl::BindBuffer(Gl::ARRAY_BUFFER, state->vertexBuffer);
    Gl::EnableVertexAttribArray(0);
    Gl::VertexAttribPointer(0, 2, Gl::FLOAT, Gl::FALSE_VALUE, sizeof(PcOverlayVertex),
                            (const void*)offsetof(PcOverlayVertex, x));
    Gl::EnableVertexAttribArray(1);
    Gl::VertexAttribPointer(1, 2, Gl::FLOAT, Gl::FALSE_VALUE, sizeof(PcOverlayVertex),
                            (const void*)offsetof(PcOverlayVertex, u));
    Gl::EnableVertexAttribArray(2);
    Gl::VertexAttribPointer(2, 4, Gl::UNSIGNED_BYTE, Gl::TRUE_VALUE, sizeof(PcOverlayVertex),
                            (const void*)offsetof(PcOverlayVertex, colour));
    Gl::BindVertexArray(0);
    state->available = true;
    return true;
}

void PcOverlayShutdown(PcOverlayState* state)
{
    if (!state->initialised)
        return;
    if (state->vertexBuffer != 0)
        Gl::DeleteBuffers(1, &state->vertexBuffer);
    if (state->vertexArray != 0)
        Gl::DeleteVertexArrays(1, &state->vertexArray);
    if (state->program != 0)
        Gl::DeleteProgram(state->program);
    *state = PcOverlayState();
}

void PcOverlayBegin(const PcOverlayState& state, GLuint framebuffer, int width, int height, bool flipY)
{
    Gl::BindFramebuffer(Gl::FRAMEBUFFER, framebuffer);
    Gl::Viewport(0, 0, width, height);
    Gl::Disable(Gl::DEPTH_TEST);
    Gl::Disable(Gl::STENCIL_TEST);
    Gl::Disable(Gl::SCISSOR_TEST);
    Gl::Disable(Gl::CULL_FACE);
    Gl::ColorMask(Gl::TRUE_VALUE, Gl::TRUE_VALUE, Gl::TRUE_VALUE, Gl::TRUE_VALUE);
    Gl::Enable(Gl::BLEND);
    Gl::BlendFuncSeparate(Gl::SRC_ALPHA, Gl::ONE_MINUS_SRC_ALPHA, Gl::ONE, Gl::ONE_MINUS_SRC_ALPHA);

    Gl::UseProgram(state.program);
    Gl::Uniform2f(state.screenLocation, (float)width, (float)height);
    Gl::Uniform1f(state.flipLocation, flipY ? -1.0f : 1.0f);
    Gl::Uniform1i(state.samplerLocation, 0);
    Gl::ActiveTexture(Gl::TEXTURE0);
    Gl::BindSampler(0, 0);
    Gl::BindVertexArray(state.vertexArray);
    Gl::BindBuffer(Gl::ARRAY_BUFFER, state.vertexBuffer);
}

void PcOverlayBatch(GLuint texture, const std::vector<PcOverlayVertex>& vertices)
{
    if (vertices.empty() || texture == 0)
        return;
    Gl::BindTexture(Gl::TEXTURE_2D, texture);
    Gl::BufferData(Gl::ARRAY_BUFFER, (GLsizeiptr)(vertices.size() * sizeof(PcOverlayVertex)), vertices.data(),
                   Gl::STREAM_DRAW);
    Gl::DrawArrays(Gl::TRIANGLES, 0, (GLsizei)vertices.size());
}

void PcOverlayEnd()
{
    Gl::BindVertexArray(0);
}

void PcOverlayPushQuad(std::vector<PcOverlayVertex>& vertices, float x0, float y0, float x1, float y1, float u0,
                       float v0, float u1, float v1, unsigned char red, unsigned char green, unsigned char blue,
                       unsigned char alpha)
{
    PcOverlayVertex corners[4] = {{x0, y0, u0, v0, {red, green, blue, alpha}},
                                  {x1, y0, u1, v0, {red, green, blue, alpha}},
                                  {x1, y1, u1, v1, {red, green, blue, alpha}},
                                  {x0, y1, u0, v1, {red, green, blue, alpha}}};
    vertices.push_back(corners[0]);
    vertices.push_back(corners[1]);
    vertices.push_back(corners[2]);
    vertices.push_back(corners[0]);
    vertices.push_back(corners[2]);
    vertices.push_back(corners[3]);
}
