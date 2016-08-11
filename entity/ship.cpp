#include "ship.h"
#include "weapon/weapon.h"
#ifndef NDEBUG
  #include <iostream>
#endif // NDEBUG

ship::ship(world &this_parent_world,
           chunk *parent_chunk,
           vector3f const &this_position,
           quatf const &this_orientation)
  : entity(this_parent_world, parent_chunk, this_position, this_orientation) {
  /// Default constructor
  mass = 10000.0;
  drag = 3.0 * 5.0 * 5.0;
  radius = 5.0;
}

ship::~ship() {
  /// Default destructor
  for(auto const &w : weapons) {
    delete w;
  }
}

vector3f ship::get_weapon_position() {
  /// Return the vector position of the weapon's mount point
  vector3f offset(0.0, -0.5, -2.0);
  offset.rotate(orientation_conjugate);
  offset += position;
  return offset;
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

void ship::accelerate(vector3f const &accel) {
  /// Accelerate this ship along a given vector relative to its orientation
  auto accel_copy(accel);
  accel_copy.rotate(orientation_conjugate);
  velocity += accel_copy * acceleration;
}

void ship::rotate(float yaw, float pitch) {
  /// Rotate this ship by yaw and pitch angles relative to its orientation
  vector3f axis_yaw(  0.0f, 1.0f, 0.0f);
  vector3f axis_pitch(1.0f, 0.0f, 0.0f);
  axis_yaw.rotate(  orientation_conjugate);
  axis_pitch.rotate(orientation_conjugate);
  orientation *= quatf::from_axis_rot(axis_yaw,   yaw);
  orientation *= quatf::from_axis_rot(axis_pitch, pitch);

  orientation_conjugate = orientation;                                          // update the cached conjugate
  orientation_conjugate.conjugate();
}

void ship::roll(float roll) {
  /// Rotate this ship through a roll angle relative to its orientation
  vector3f axis_roll(0.0f, 0.0f, 1.0f);
  axis_roll.rotate(orientation_conjugate);
  orientation *= quatf::from_axis_rot(axis_roll, roll);

  orientation_conjugate = orientation;                                          // update the cached conjugate
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

