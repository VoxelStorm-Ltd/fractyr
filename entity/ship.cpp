#include "ship.h"

ship::ship(world &parent_world, chunk *parent_chunk, Vector3f const &position)
  : entity(parent_world, parent_chunk, position),
    orientation_conjugate(orientation) {
  /// Default constructor
  orientation_conjugate.conjugate();
}

ship::~ship() {
  /// Default destructor
}

void ship::accelerate(Vector3f accel) {
  /// Accelerate this ship along a given vector relative to its orientation
  accel.rotate(orientation_conjugate);
  velocity += accel * acceleration;
}

void ship::rotate(float yaw, float pitch) {
  /// Rotate this ship by yaw and pitch angles relative to its orientation
  Vector3f axis_yaw(0.0, 1.0, 0.0);
  Vector3f axis_pitch(1.0, 0.0, 0.0);
  axis_yaw.rotate(  orientation_conjugate);
  axis_pitch.rotate(orientation_conjugate);
  orientation *= Quatf::fromAxisRot(axis_yaw,   yaw);
  orientation *= Quatf::fromAxisRot(axis_pitch, pitch);

  orientation_conjugate = orientation;    // update the cached conjugate
  orientation_conjugate.conjugate();
}
