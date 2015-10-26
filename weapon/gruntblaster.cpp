#include "gruntblaster.h"
#include "entity/bullet/plasma.h"
#include "entity/ship.h"

gruntblaster::gruntblaster(ship *parent)
  : weapon(parent) {
  /// Default constructor
}

gruntblaster::~gruntblaster() {
  /// Default destructor
}

bool gruntblaster::fire() {
  /// Fire this weapon
  if(cooldown != 0) {
    return false;
  }
  // position this at our ship
  Vector3f const &barrel_coords(parent->get_weapon_position());
  Quatf const &barrel_orientation(parent->get_orientation());
  Quatf const &barrel_orientation_conjugate(parent->get_orientation_conjugate());
  chunk *thischunk(parent->get_parent());
  Vector3f barrel_coords_left(1.0, 0.0, -parent->radius - 0.3);
  barrel_coords_left.rotate(barrel_orientation_conjugate);
  barrel_coords_left += barrel_coords;

  Vector3f barrel_coords_right(-1.0, 0.0, -parent->radius - 0.3);
  barrel_coords_right.rotate(barrel_orientation_conjugate);
  barrel_coords_right += barrel_coords;

  parent->correct_point(barrel_coords_left,  thischunk);
  parent->correct_point(barrel_coords_right, thischunk);

  new plasma(*parent->get_parent_world(), thischunk, barrel_coords_left,  barrel_orientation, parent->get_velocity(), get_shot_speed());
  new plasma(*parent->get_parent_world(), thischunk, barrel_coords_right, barrel_orientation, parent->get_velocity(), get_shot_speed());
  cooldown = fire_rate;
  return true;
}

unsigned int gruntblaster::get_cost_per_shot() const {
  return 25;
}

float gruntblaster::get_shot_speed() const {
  return 0.9;
}
