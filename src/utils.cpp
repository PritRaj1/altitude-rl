#include "utils.hpp"
#include "env.hpp"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void log2csv(LanderController controller, const string &filename) {
  cout << "Logging to " << filename << "\n";
  ofstream csv_file(filename);
  csv_file << "step,altitude,velocity,fuel,thrust,reward,is_terminal\n";

  MarsLanderEnv env;
  env.reset();
  LanderState state = env.get_state();

  int step = 0;
  double cumulative_reward = 0.0;
  while (true) {
    double thrust = controller(state);
    double reward = env.calculate_reward(thrust);
    cumulative_reward += reward;
    bool terminal = env.is_terminal();

    csv_file << step << "," << state.altitude << "," << state.velocity << ","
             << state.fuel << "," << thrust << "," << reward << ","
             << (terminal ? 1 : 0) << "\n";

    if (terminal) {
      break;
    }

    env.step(thrust);
    state = env.get_state();
    step++;
  }

  csv_file.close();
  cout << "Finished logging:";
  cout << "  Steps: " << step << " | Acc Reward: " << cumulative_reward << "\n";
};
