#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

struct LanderState {
    double altitude = 100.0;
    double velocity = 0.0;
    double mass = 800.0;
};

int main() {
  LanderState lander;
  const double MARS_G = -3.71;
  const double dt = 0.1;

  while (lander.altitude > 0.0) {
    lander.velocity += MARS_G * dt;
    lander.altitude += lander.velocity * dt;

    cout << "Altitude: " << fixed << lander.altitude << "m\t" << "Velocity: " << lander.velocity << "m/s\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }

  cout << "Final Impact Velocity: " << lander.velocity << " m/s\n";
  return 0;
}
