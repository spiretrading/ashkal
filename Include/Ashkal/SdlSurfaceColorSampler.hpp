#ifndef ASHKAL_SDL_SURFACE_COLOR_SAMPLER_HPP
#define ASHKAL_SDL_SURFACE_COLOR_SAMPLER_HPP
#include <filesystem>
#include <SDL.h>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/ColorSampler.hpp"

namespace Ashkal {
  class SdlSurfaceColorSampler : public ColorSampler {
    public:
      explicit SdlSurfaceColorSampler(SDL_Surface& surface);

      ~SdlSurfaceColorSampler() override;

      Color sample(const TextureCoordinate& uv) const override;

    private:
      SDL_Surface* m_surface;
  };

  inline std::shared_ptr<SdlSurfaceColorSampler> load_bitmap_sampler(
      const std::filesystem::path& path) {
    auto surface = SDL_LoadBMP(path.string().c_str());
    if(!surface) {
      throw std::runtime_error("Texture not found.");
    }
    return std::make_shared<SdlSurfaceColorSampler>(*surface);
  }

  inline SdlSurfaceColorSampler::SdlSurfaceColorSampler(SDL_Surface& surface)
    : m_surface(&surface) {}

  inline SdlSurfaceColorSampler::~SdlSurfaceColorSampler() {
    SDL_FreeSurface(m_surface);
  }

  Color SdlSurfaceColorSampler::sample(const TextureCoordinate& uv) const {
    auto x = static_cast<int>(uv.m_u * (m_surface->w - 1));
    auto y = static_cast<int>((1 - uv.m_v) * (m_surface->h - 1));
    if(SDL_MUSTLOCK(m_surface)) {
      SDL_LockSurface(m_surface);
    }
    auto bpp = m_surface->format->BytesPerPixel;
    auto pixel_ptr =
      static_cast<Uint8*>(m_surface->pixels) + y * m_surface->pitch + x * bpp;
    auto pixel = Uint32(0);
    if(bpp == 1) {
      pixel = *pixel_ptr;
    } else if(bpp == 2) {
      pixel = *reinterpret_cast<Uint16*>(pixel_ptr);
    } else if(bpp == 3) {
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        pixel = pixel_ptr[0] << 16 | pixel_ptr[1] << 8 | pixel_ptr[2];
      } else {
        pixel = pixel_ptr[0] | pixel_ptr[1] << 8 | pixel_ptr[2] << 16;
      }
    } else if(bpp == 4) {
      pixel = *reinterpret_cast<Uint32*>(pixel_ptr);
    } else {
      pixel = 0;
    }
    if(SDL_MUSTLOCK(m_surface)) {
      SDL_UnlockSurface(m_surface);
    }
    auto r = Uint8();
    auto g = Uint8();
    auto b = Uint8();
    auto a = Uint8();
    SDL_GetRGBA(pixel, m_surface->format, &r, &g, &b, &a);
    return Color(r, g, b, a);
  }
}

#endif
