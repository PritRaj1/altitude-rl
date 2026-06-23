#include "agent.hpp"
#include "env.hpp"
#include "utils.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

vector<double> train_agent(TDtype type, const string &filename,
                           int num_episodes = 1500000) {
  ofstream csv_file(filename);
  csv_file << "seed,episode,reward,velocity\n";

  string run_name = (type == TDtype::QLearning) ? "Q-Learning" : "SARSA";
  cout << "Starting training: " << run_name << "\n";

  const int EVAL_INTERVAL = 100000;
  vector<int> seeds = {42, 1337, 2026, 999, 7};

  vector<double> trained_weights;
  for (int seed : seeds) {
    srand(seed);
    MarsLanderEnv env;
    Agent agent(0.01, 1.0, 1.0, env);

    for (int ep = 0; ep < num_episodes; ++ep) {

      // Eval
      if (ep % EVAL_INTERVAL == 0) {
        MarsLanderEnv eval_env;
        eval_env.reset();
        double eval_cum_reward = 0.0;

        LanderState eval_state = eval_env.get_state();
        while (!eval_env.is_terminal()) {
          int action =
              agent.choose_action(eval_state, true); // true = pure greedy
          double thrust = agent.get_thrust(action);
          eval_env.step(thrust);

          eval_cum_reward += eval_env.calculate_reward(thrust);
          eval_state = eval_env.get_state();
        }
        csv_file << seed << "," << ep << "," << eval_cum_reward << ","
                 << eval_env.get_state().velocity << "\n";
      }

      // Train
      env.reset();
      LanderState state = env.get_state();
      int action = agent.choose_action(state);

      while (!env.is_terminal()) {
        double thrust = agent.get_thrust(action);
        env.step(thrust);
        LanderState next_state = env.get_state();
        double reward = env.calculate_reward(thrust);

        int next_action = agent.choose_action(next_state);
        agent.update(type, state, action, reward, next_state, next_action);
        state = next_state;
        action = next_action;
      }
      agent.decay_epsilon(0.999);
    }
    cout << "  -> Finished tracking Seed [" << seed << "]\n";
    trained_weights = agent.get_q_table();
  }
  cout << "Finished all seeds for " << run_name << ". Output to " << filename
       << "\n\n";
  return trained_weights;
}

void run(vector<double> &weights) {
  MarsLanderEnv env;
  LanderState state = env.get_state();
  Agent agent(0.01, 1.0, 1.0, env);
  agent.set_q_table(weights);

  while (!env.is_terminal()) {
    int action = agent.choose_action(state, true);
    double thrust = agent.get_thrust(action);
    draw_lander(state.altitude, thrust, env.MAX_THRUST);
    this_thread::sleep_for(chrono::milliseconds(50));
    env.step(thrust);
    state = env.get_state();
  }

  if (abs(state.velocity) < 5.0) {
    cout << "Safe landing at: " << state.velocity << " m/s\n";
  } else {
    cout << "Crash landed at: " << state.velocity << " m/s\n";
  }
}

int main() {
  vector<double> q_learned =
      train_agent(TDtype::QLearning, "q_learning_curves.csv");
  vector<double> sarsa =
      train_agent(TDtype::SARSA, "sarsa_learning_curves.csv");

  cout << "Press [ENTER] for Q-learning sim\n";
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  cin.get();
  run(q_learned);

  cout << "\n\nPress [ENTER] for SARSA sim\n";
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  cin.get();
  run(sarsa);

  return 0;
}
