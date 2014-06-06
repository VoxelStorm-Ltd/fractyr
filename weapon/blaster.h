#ifndef BLASTER_H_INCLUDED
#define BLASTER_H_INCLUDED

#include "weapon.h"

class blaster : public weapon {
  /// Basic classic video game slow-moving bullet blaster
private:
  float fire_rate = 10;

public:
  blaster(ship *parent);
  ~blaster();

  bool fire() override final;
  unsigned int get_cost_per_shot() const;
  float get_shot_speed() const;
};

#endif // BLASTER_H_INCLUDED
