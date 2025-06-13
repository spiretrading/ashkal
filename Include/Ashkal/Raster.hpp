#ifndef ASHKAL_RASTER_HPP
#define ASHKAL_RASTER_HPP
#include <algorithm>
#include <vector>

namespace Ashkal {

  /**
   * A two-dimensional raster of values of type T, stored in row-major order.
   * @param <T> The type of data stored.
   */
  template<typename T>
  class Raster {
    public:

      /** The type of data stored. */
      using Type = T;

      /**
       * Constructs a raster of the given width and height.
       * @param width Number of columns.
       * @param height Number of rows.
       */
      Raster(int width, int height);

      /** Returns the raster width (number of columns). */
      int get_width() const;

      /** Returns the raster height (number of rows). */
      int get_height() const;

      /** Returns total number of elements (width * height). */
      int get_size() const;

      /** Returns total buffer size in bytes. */
      int get_byte_size() const;

      /**
       * Read-only element access at (x, y).
       * @param x Column index [0..width-1]
       * @param y Row index [0..height-1]
       */
      Type operator()(int x, int y) const;

      /**
       * Mutable element access at (x, y).
       */
      Type& operator()(int x, int y);

      /** Fill the entire raster with the given value. */
      void fill(T value);

      /** Pointer to the underlying contiguous data array. */
      const Type* data() const;

    private:
      int m_width;
      int m_height;
      std::vector<Type> m_buffer;
  };

  template<typename T>
  Raster<T>::Raster(int width, int height)
    : m_width(width),
      m_height(height),
      m_buffer(width * height) {}

  template<typename T>
  int Raster<T>::get_width() const {
    return m_width;
  }

  template<typename T>
  int Raster<T>::get_height() const {
    return m_height;
  }

  template<typename T>
  int Raster<T>::get_size() const {
    return m_width * m_height;
  }

  template<typename T>
  int Raster<T>::get_byte_size() const {
    return get_size() * static_cast<int>(sizeof(T));
  }

  template<typename T>
  typename Raster<T>::Type Raster<T>::operator()(int x, int y) const {
    return m_buffer[y * m_width + x];
  }

  template<typename T>
  typename Raster<T>::Type& Raster<T>::operator()(int x, int y) {
    return m_buffer[y * m_width + x];
  }

  template<typename T>
  void Raster<T>::fill(T value) {
    std::fill(m_buffer.begin(), m_buffer.end(), value);
  }

  template<typename T>
  const typename Raster<T>::Type* Raster<T>::data() const {
    return m_buffer.data();
  }
}

#endif
