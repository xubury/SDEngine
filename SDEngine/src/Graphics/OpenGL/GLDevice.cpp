#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/VertexArray.hpp"
#include <GL/glew.h>

namespace SD {

static void OpenGLMessageCallback(GLenum, GLenum, unsigned, GLenum severity,
                                  int, const char *message, const void *)
{
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            SD_CORE_ASSERT(false, message);
            SD_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            // SD_CORE_ASSERT(false, message);
            SD_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_LOW:
            SD_CORE_WARN(message);
            return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            SD_CORE_TRACE(message);
            return;
        default:
            SD_CORE_ASSERT(false, "Unknown severity level!");
    }
}

GLDevice::GLDevice()
{
    SD_CORE_ASSERT(glewInit() == GLEW_OK, "glewInit failed!");

    SD_CORE_INFO("---Graphics Card Info---");
    SD_CORE_INFO("Vendor: {}", glGetString(GL_VENDOR));
    SD_CORE_INFO("Renderer: {}", glGetString(GL_RENDERER));
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

    Enable(Operation::MSAA);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLDevice::DispatchCompute(int32_t num_group_x, int32_t num_group_y,
                               int32_t num_group_z)
{
    glDispatchCompute(num_group_x, num_group_y, num_group_z);
}

void GLDevice::MemoryBarrier(BarrierBit bit)
{
    glMemoryBarrier(Translate(bit));
}

void GLDevice::DrawElements(MeshTopology topology, int count, size_t offset)
{
    glDrawElements(Translate(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::DrawElementsInstanced(MeshTopology topology, int count,
                                     size_t offset, size_t amount)
{
    glDrawElementsInstanced(Translate(topology), count, GL_UNSIGNED_INT,
                            (const void *)offset, amount);
}

void GLDevice::DrawArrays(MeshTopology topology, int first, int count)
{
    glDrawArrays(Translate(topology), first, count);
}

void GLDevice::SetLineWidth(float width) { glLineWidth(width); }

void GLDevice::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void GLDevice::Clear(BufferBitMask bit)
{
    // glUseProgram(0);
    glClear(Translate(bit & BufferBitMask::ColorBufferBit) |
            Translate(bit & BufferBitMask::DepthBufferBit) |
            Translate(bit & BufferBitMask::StencilBufferBit));
}

void GLDevice::SetVertexArray(const VertexArray *vertexArray)
{
    if (vertexArray) {
        glBindVertexArray(vertexArray->Handle());
    }
    else {
        glBindVertexArray(0);
    }
}

void GLDevice::SetShader(const Shader *shader)
{
    if (shader) {
        glUseProgram(shader->Handle());
    }
    else {
        glUseProgram(0);
    }
}

void GLDevice::SetViewport(int x, int y, int width, int height)
{
    // opengl define viewport origin at bottom-left
    glViewport(x, y, width, height);
}

void GLDevice::SetFramebuffer(const Framebuffer *framebuffer)
{
    if (framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->Handle());
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GLDevice::SetPolygonMode(PolygonMode mode, Face face)
{
    glPolygonMode(Translate(face), Translate(mode));
}

void GLDevice::SetDepthMask(bool depth_mask) { glDepthMask(depth_mask); }

void GLDevice::Disable(Operation operation) { glDisable(Translate(operation)); }

void GLDevice::Enable(Operation operation) { glEnable(Translate(operation)); }

void GLDevice::SetCullFace(Face face) { glCullFace(Translate(face)); }

void GLDevice::SetDepthfunc(DepthFunc depth_func)
{
    glDepthFunc(Translate(depth_func));
}

void GLDevice::DrawBuffer(Framebuffer *fb, int buf)
{
    if (fb) {
        glNamedFramebufferDrawBuffer(fb->Handle(), GL_COLOR_ATTACHMENT0 + buf);
    }
    else {
        glNamedFramebufferDrawBuffer(0, GL_FRONT_LEFT + buf);
    }
}

void GLDevice::DrawBuffers(Framebuffer *fb, int n, const int *buf)
{
    std::vector<GLenum> glbuf(n);
    if (fb) {
        std::generate(glbuf.begin(), glbuf.end(), [i = 0, buf]() mutable {
            return buf[i++] + GL_COLOR_ATTACHMENT0;
        });
        glNamedFramebufferDrawBuffers(fb->Handle(), n, glbuf.data());
    }
    else {
        std::generate(glbuf.begin(), glbuf.end(), [i = 0, buf]() mutable {
            return buf[i++] + GL_FRONT_LEFT;
        });
        glNamedFramebufferDrawBuffers(0, n, glbuf.data());
    }
}

void GLDevice::ReadBuffer(const Framebuffer *fb, int buf)
{
    if (fb) {
        glNamedFramebufferReadBuffer(fb->Handle(), GL_COLOR_ATTACHMENT0 + buf);
    }
    else {
        glNamedFramebufferReadBuffer(0, GL_FRONT_LEFT + buf);
    }
}

void GLDevice::BlitFramebuffer(const Framebuffer *src, int src_x, int src_y,
                               int src_width, int src_height, Framebuffer *dst,
                               int dst_x, int dst_y, int dst_width,
                               int dst_height, BufferBitMask mask,
                               BlitFilter filter)
{
    uint32_t src_id = src ? src->Handle() : 0;
    uint32_t dst_id = dst ? dst->Handle() : 0;
    GLint gl_mask = Translate(mask & BufferBitMask::ColorBufferBit) |
                    Translate(mask & BufferBitMask::DepthBufferBit) |
                    Translate(mask & BufferBitMask::StencilBufferBit);
    GLenum gl_filter = Translate(filter);
    glBlitNamedFramebuffer(src_id, dst_id, src_x, src_y, src_x + src_width,
                           src_y + src_height, dst_x, dst_y, dst_x + dst_width,
                           dst_y + dst_height, gl_mask, gl_filter);
}

void GLDevice::ReadPixels(int x, int y, int width, int height,
                          DataFormat format, void *data)
{
    glReadPixels(x, y, width, height, GetFormatType(format),
                 GetDataType(format), data);
}

}  // namespace SD
