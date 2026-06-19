#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

struct LanderState {
  double altitude = 100.0;
  double velocity = -50.0;
  const double mass = 800.0;
  double fuel = 200.0;
  double thrust = 0.0;
};

int main() {
  LanderState lander;
  const double MARS_G = -3.71;
  const double MAX_THRUST = 12000.0;
  const double FUEL_BURN_RATE = 0.05;
  const double dt = 0.1;

  while (lander.altitude > 0.0) {
    if (lander.altitude < 500.0 && lander.fuel > 0.0) {
      lander.thrust = 7000.0; // Engines firing
    } else {
      lander.thrust = 0.0; // Engines idle
    }
    double fuel_needed = lander.thrust * FUEL_BURN_RATE * dt;
    if (fuel_needed > lander.fuel) {
      fuel_needed = lander.fuel;
      lander.thrust = fuel_needed / (FUEL_BURN_RATE * dt);
    }
    lander.fuel -= fuel_needed;

    double total_mass = lander.mass + lander.fuel;
    double acceleration = (lander.thrust / total_mass) + MARS_G;

    lander.velocity += acceleration * dt;
    lander.altitude += lander.velocity * dt;

    if (lander.altitude < 0.0) {
      lander.altitude = 0.0;
    }

    cout << "Altitude: " << fixed << lander.altitude << "m\t"
         << "Velocity: " << lander.velocity << "m/s\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }

  cout << "Final Impact Velocity: " << lander.velocity << " m/s\n";
  if (abs(lander.velocity) < 5.0) {
    cout << "Safe landing at " << lander.velocity << " m/s\n";
  } else {
    cout << "Crashed at " << lander.velocity << " m/s\n";
  }

  return 0;
}
