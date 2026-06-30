#include "agent.hpp"
#include "control.hpp"
#include "env.hpp"
#include "train.hpp"
#include "utils.hpp"

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int main() {
  double ALPHA = 0.001;
  double GAMMA = 1.0;
  double EPSILON = 1.0;
  double DECAY = 0.99999;

  double Kp_vel = 350.0;
  double Ki_vel = 0.0;
  double Kd_vel = 80.0;
  double Kp_alt = 0.12;

  const int NUM_THREADS = 8;
  const int EPISODES_PER_WORKER = 10000;
  TDtype td_type = TDtype::QLearning;

  MarsLanderEnv env;

  Agent global_agent(ALPHA, GAMMA, EPSILON, DECAY, env);
  ThreadSafeReplayBuffer replay_buffer;
  atomic<bool> training_active(true);

  jthread learner_thread(global_optim, ref(global_agent), ref(replay_buffer),
                         ref(training_active), td_type);

  vector<jthread> workers;
  for (int i = 0; i < NUM_THREADS; ++i) {
    workers.emplace_back(local_rollout, i, ref(replay_buffer),
                         ref(global_agent), ref(training_active),
                         EPISODES_PER_WORKER);
  }

  for (auto &w : workers) {
    if (w.joinable())
      w.join();
  }

  cout << "All threads synchronised.\n";
  replay_buffer.deactivate(training_active);

  if (learner_thread.joinable())
    learner_thread.join();

  cout << "Global Q-Table compiled.\n\n";

  auto rl_controller = [&global_agent](const LanderState &s) -> double {
    int action = global_agent.choose_action(s, true); // true = pure greedy
    return global_agent.get_thrust(action);
  };
  log2csv(rl_controller, "q_learning.csv");

  cout << "Logged to q_leaning.csv, starting PID.";

  env.reset();
  CascadedController cascaded_pid(0.0, Kp_vel, Ki_vel, Kd_vel, Kp_alt, env);
  auto pid_controller = [&cascaded_pid](const LanderState &s) -> double {
    return cascaded_pid.action(s);
  };
  log2csv(pid_controller, "pid.csv");

  cout << "Logged to pid.csv, All jobs finished.";
  return 0;
}
