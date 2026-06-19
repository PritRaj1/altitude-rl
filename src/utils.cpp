#include "utils.hpp"
#include <iostream>
#include <string>

using namespace std;

void draw_lander(double altitude, double thrust, double max_thrust) {
    const int MAX_HEIGHT_LINES = 20;
    int lander_row = MAX_HEIGHT_LINES - static_cast<int>((altitude / 100.0) * MAX_HEIGHT_LINES);
    
    // Bounds check
    if (lander_row < 0) lander_row = 0;
    if (lander_row >= MAX_HEIGHT_LINES) lander_row = MAX_HEIGHT_LINES - 1;

    // Flames based on thrust effort
    double throttle = thrust / max_thrust;
    string flame = " ";
    if (throttle > 0.1)  flame = "v";
    if (throttle > 0.4)  flame = "▼";
    if (throttle > 0.8)  flame = "█";

    for (int i = 0; i < MAX_HEIGHT_LINES; ++i) {
        if (i == lander_row) {
            cout << "  [X]   <-- Lander\n";
            cout << "   " << flame << " " << thrust << " N\n";
        } else {
            cout << "   | \n";
        }
    }
    cout << "====== MARS SURFACE ======\n";
    cout << "Altitude: " << fixed << altitude << " m\n\n";

}
