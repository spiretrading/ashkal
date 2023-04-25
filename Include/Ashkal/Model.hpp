#ifndef ASHKAL_MODEL_HPP
#define ASHKAL_MODEL_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Voxel.hpp"

namespace Ashkal {

  /** Stores a model made up of voxels. */
  class Model {
    public:
      virtual ~Model() = default;

      /**
       * Returns the point representing the top-right corner of the model's
       * bounding box. The point returned is exclusive.
       */
      virtual Point end() const = 0;

      /** Returns the voxel at a specified point. */
      virtual Voxel get(Point point) const = 0;

    protected:

      /** Constructs an empty Model. */
      Model() = default;
  };
}

#endif
