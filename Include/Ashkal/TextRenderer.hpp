#ifndef ASHKAL_TEXT_RENDERER_HPP
#define ASHKAL_TEXT_RENDERER_HPP
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <SDL_ttf.h>
#include "Ashkal/Color.hpp"
#include "Ashkal/Raster.hpp"

namespace Ashkal {

  /** Implements a renderer for text. */
  class TextRenderer {
    public:

      /**
       * Constructs the TextRenderer with given font file and size.
       * Initializes SDL_ttf and loads the font.
       * @param path The path to the font file.
       * @param size The font size.
       */
      TextRenderer(const std::filesystem::path& path, int size);

      ~TextRenderer();

      /**
       * Renders the given text into the provided frame buffer at (x, y) with a
       * specified color.
       * @param text The text to render.
       * @param x The x coordinate to render to.
       * @param y The y coordinate to render to.
       * @param color The color of the text to render.
       * @param frame_buffer The FrameBuffer to render to.
       */
      void render(std::string_view text, int x, int y, Color color,
        FrameBuffer& frame_buffer);

  private:
    TTF_Font* m_font;
    int m_line_skip;

    void render_line(const std::string& line, int x, int y, Color color,
      FrameBuffer& frame_buffer);
  };

  inline TextRenderer::TextRenderer(
      const std::filesystem::path& path, int size) {
    if(TTF_WasInit() == 0) {
      if(TTF_Init() != 0) {
        throw std::runtime_error("TTF_Init failed.");
      }
    }
    m_font = TTF_OpenFont(path.string().c_str(), size);
    if(!m_font) {
      throw std::runtime_error("Failed to load font.");
    }
    m_line_skip = TTF_FontLineSkip(m_font);
  }

  inline TextRenderer::~TextRenderer() {
    if(m_font) {
      TTF_CloseFont(m_font);
    }
  }

  inline void TextRenderer::render(std::string_view text, int x, int y,
      Color color, FrameBuffer& frame_buffer) {
    auto position = std::size_t(0);
    while(position < text.size()) {
      auto next = text.find('\n', position);
      auto line = text.substr(position, next - position);
      render_line(std::string(line), x, y, color, frame_buffer);
      if(next == std::string::npos) {
        break;
      }
      y += float(m_line_skip);
      position = next + 1;
    }
  }

  inline void TextRenderer::render_line(const std::string& line, int x, int y,
      Color color, FrameBuffer& frame_buffer) {
    if(line.empty()) {
      return;
    }
    auto sdl_color = SDL_Color(
      color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
    auto surface = TTF_RenderText_Blended(m_font, line.c_str(), sdl_color);
    if(!surface) {
      return;
    }
    if(SDL_LockSurface(surface) != 0) {
      SDL_FreeSurface(surface);
      return;
    }
    auto pixels = static_cast<Uint32*>(surface->pixels);
    auto format = surface->format;
    for(auto row = 0; row < surface->h; ++row) {
      for(auto column = 0; column < surface->w; ++column) {
        auto pixel = pixels[row * surface->w + column];
        auto source_red = Uint8();
        auto source_green = Uint8();
        auto source_blue = Uint8();
        auto source_alpha = Uint8();
        SDL_GetRGBA(pixel, format, &source_red, &source_green, &source_blue,
          &source_alpha);
        if(source_alpha == 0) {
          continue;
        }
        auto frame_x = x + column;
        auto frame_y = y + row;
        if(frame_x < 0 || frame_x >= frame_buffer.get_width() || frame_y < 0 ||
            frame_y >= frame_buffer.get_height()) {
          continue;
        }
        auto destination = frame_buffer(frame_x, frame_y);
        auto alpha = source_alpha / 255.f;
        auto out_red = static_cast<Uint8>(
          source_red * alpha + destination.get_red() * (1 - alpha));
        auto out_green = static_cast<Uint8>(
          source_green * alpha + destination.get_green() * (1 - alpha));
        auto out_blue = static_cast<Uint8>(
          source_blue * alpha + destination.get_blue() * (1 - alpha));
        auto out_alpha = static_cast<Uint8>(
          source_alpha + destination.get_alpha() * (1 - alpha));
        frame_buffer(frame_x, frame_y) = Color(out_red, out_green, out_blue, out_alpha);
      }
    }
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);
  }
}

#endif
