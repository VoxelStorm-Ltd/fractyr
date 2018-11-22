#ifndef GRUNTBLASTER_H_INCLUDED
#define GRUNTBLASTER_H_INCLUDED

#include "weapon.h"

class gruntblaster : public weapon {
  /// Basic classic video game slow-moving bullet blaster
private:
  float fire_rate = 40;

public:
  gruntblaster(ship *parent);
  ~gruntblaster();

  bool fire() override final;
  unsigned int get_cost_per_shot() const override final __attribute__((__const__));
  float get_shot_speed() const override final __attribute__((__const__));
};

#endif // GRUNTBLASTER_H_INCLUDED
