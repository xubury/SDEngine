#ifndef SD_DEVICE_HPP
#define SD_DEVICE_HPP

#include "Core/Base.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

class Device {
   public:
    static API getAPI();

    virtual ~Device() = default;

    static void init(API api);

   protected:
    Device(const Device &) = delete;

    Device &operator=(const Device &) = delete;

    virtual void drawElements(MeshTopology topology, size_t count,
                              size_t offset) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual void clear() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    Device() = default;

   private:
    friend class RendererAPI;

    static Device &instance();

    static API s_api;
    static Scope<Device> s_instance;
};

}  // namespace sd

#endif /* SD_DEVICE_HPP */
