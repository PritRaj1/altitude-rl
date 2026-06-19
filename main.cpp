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
    lander.velocity += MARS_G * dt;
    lander.altitude += lander.velocity * dt;

    cout << "Altitude: " << fixed << lander.altitude << "m\t"
         << "Velocity: " << lander.velocity << "m/s\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }

  cout << "Final Impact Velocity: " << lander.velocity << " m/s\n";
  return 0;
}
