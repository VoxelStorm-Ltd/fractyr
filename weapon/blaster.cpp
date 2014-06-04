#include "blaster.h"
#include "plasma.h"
#include "ship.h"

blaster::blaster(ship *parent)
  : weapon(parent) {
  /// Default constructor
}

blaster::~blaster() {
  /// Default destructor
}

void blaster::fire() {
  /// Fire this weapon
  if(cooldown != 0) {
    return;
  }
  // position this at our ship
  Vector3f const &barrel_coords(parent->get_weapon_position());
  Quatf const &barrel_orientation(parent->get_orientation());
  Quatf const &barrel_orientation_conjugate(parent->get_orientation_conjugate());
  chunk *thischunk(parent->get_parent());
  Vector3f barrel_offset(1.0, 0.0, 0.0);
  barrel_offset.rotate(barrel_orientation_conjugate);

  Vector3f barrel_coords_left( barrel_coords - barrel_offset);
  Vector3f barrel_coords_right(barrel_coords + barrel_offset);

  parent->correct_point(barrel_coords_left,  thischunk);
  parent->correct_point(barrel_coords_right, thischunk);

  new plasma(*parent->get_parent_world(), thischunk, barrel_coords_left,  barrel_orientation, parent->get_velocity());
  new plasma(*parent->get_parent_world(), thischunk, barrel_coords_right, barrel_orientation, parent->get_velocity());
  cooldown = 60 / 10;
}
