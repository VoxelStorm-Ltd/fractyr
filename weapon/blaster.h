#ifndef BLASTER_H_INCLUDED
#define BLASTER_H_INCLUDED

#include "weapon.h"

class blaster : public weapon {
  /// Basic classic video game slow-moving bullet blaster
private:
  float fire_rate;
  float shot_speed;

public:
  blaster(ship *parent, float fire_rate, float shot_speed);
  ~blaster();

  bool fire() override final;
};

#endif // BLASTER_H_INCLUDED
