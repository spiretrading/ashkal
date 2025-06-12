#ifndef ASHKAL_RASTER_HPP
#define ASHKAL_RASTER_HPP
#include "Ashkal/Ashkal.hpp"

namespace Ashkal {
  template<typename T>
  class Raster {
    public:
      Raster(int width, int height);

      int get_width() const;

      int get_height() const;

      int get_size() const;

      int get_byte_size() const;

      T get(int x, int y) const;

      void set(int x, int y, T value);

      void fill(T value);

      const T* data() const;
  };
}

#endif
