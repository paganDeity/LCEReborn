#ifndef PLATFORM_PC_PCOVERLAY_HPP
#define PLATFORM_PC_PCOVERLAY_HPP

#include <vector>

#include "platform/pc/GlApi.hpp"

struct PcOverlayVertex
{
    float x;
    float y;
    float u;
    float v;
    unsigned char colour[4];
};

struct PcOverlayState
{
    bool initialised;
    bool available;
    GLuint program;
    GLint screenLocation;
    GLint flipLocation;
    GLint samplerLocation;
    GLuint vertexArray;
    GLuint vertexBuffer;
};

bool PcOverlayInitialise(PcOverlayState* state, const char* fragmentSource, const char* samplerName,
                         const char* name);
void PcOverlayShutdown(PcOverlayState* state);
void PcOverlayBegin(const PcOverlayState& state, GLuint framebuffer, int width, int height, bool flipY);
void PcOverlayBatch(GLuint texture, const std::vector<PcOverlayVertex>& vertices);
void PcOverlayEnd();
void PcOverlayPushQuad(std::vector<PcOverlayVertex>& vertices, float x0, float y0, float x1, float y1, float u0,
                       float v0, float u1, float v1, unsigned char red, unsigned char green, unsigned char blue,
                       unsigned char alpha);

#endif
