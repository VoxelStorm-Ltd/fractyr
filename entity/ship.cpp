#include "ship.h"
#include "weapon.h"

ship::ship(world &parent_world, chunk *parent_chunk,
           Vector3f const &position,
           Quatf const &orientation)
  : entity(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  mass = 10000.0;
  drag = 3.0 * 5.0 * 5.0;
  radius = 2.0;
}

ship::~ship() {
  /// Default destructor
  for(auto const &w : weapons) {
    delete w;
  }
}

Vector3f ship::get_weapon_position() {
  /// Return the vector position of the weapon's mount point
  Vector3f offset(0.0, -0.5, -2.0);
  offset.rotate(orientation_conjugate);
  offset += position;
  return offset;
}

entity::entity_type ship::get_entity_type() const {
  return entity::entity_type::SHIP;
}

void ship::update() {
  entity::update();
  for(auto const &w : weapons) {
    w->update();
  }
  orientation.normalise();
}

void ship::add_weapon(weapon *new_weapon) {
  /// Mount a weapon on this ship
  weapons.push_back(new_weapon);
}

void ship::accelerate(Vector3f accel) {
  /// Accelerate this ship along a given vector relative to its orientation
  accel.rotate(orientation_conjugate);
  velocity += accel * acceleration;
}

void ship::rotate(float yaw, float pitch) {
  /// Rotate this ship by yaw and pitch angles relative to its orientation
  Vector3f axis_yaw(  0.0f, 1.0f, 0.0f);
  Vector3f axis_pitch(1.0f, 0.0f, 0.0f);
  axis_yaw.rotate(  orientation_conjugate);
  axis_pitch.rotate(orientation_conjugate);
  orientation *= Quatf::fromAxisRot(axis_yaw,   yaw);
  orientation *= Quatf::fromAxisRot(axis_pitch, pitch);

  orientation_conjugate = orientation;    // update the cached conjugate
  orientation_conjugate.conjugate();
}

void ship::roll(float roll) {
  /// Rotate this ship through a roll angle relative to its orientation
  Vector3f axis_roll(0.0f, 0.0f, 1.0f);
  axis_roll.rotate(orientation_conjugate);
  orientation *= Quatf::fromAxisRot(axis_roll, roll);

  orientation_conjugate = orientation;    // update the cached conjugate
  orientation_conjugate.conjugate();
}

bool ship::fire(unsigned int weapon_id) {
  /// Fire the selected weapon
  #ifndef NDEBUG
    if(weapon_id >= weapons.size()) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << " tried to access index " << weapon_id << " of " << weapons.size() << std::endl;
      return false;
    }
  #endif
  return weapons[weapon_id]->fire();
}

