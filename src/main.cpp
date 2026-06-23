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
  MarsLanderEnv env;
  TDtype td_type = TDtype::SARSA;

  Agent global_agent(0.01, 1.0, 1.0, env);
  ThreadSafeReplayBuffer replay_buffer;
  atomic<bool> training_active(true);

  const int NUM_THREADS = 4;
  const int EPISODES_PER_WORKER = 100000;

  jthread learner_thread(global_optim, ref(global_agent), ref(replay_buffer),
                         ref(training_active), td_type);
  vector<jthread> workers;

  for (int i = 0; i < NUM_THREADS; ++i) {
    workers.emplace_back(local_rollout, i, ref(replay_buffer),
                         ref(training_active), EPISODES_PER_WORKER);
  }

  for (auto &w : workers) {
    if (w.joinable())
      w.join();
  }

  cout << "All threads synchronised.\n";

  training_active = false;
  if (learner_thread.joinable())
    learner_thread.join();

  cout << "Global Q-Table compiled.\n\n";

  auto rl_controller = [&global_agent](const LanderState &s) -> double {
    int action = global_agent.choose_action(s, true); // true = pure greedy
    return global_agent.get_thrust(action);
  };
  log2csv(rl_controller, "sarsa.csv");

  cout << "Logged to q_learning.csv, starting PID.";

  env.reset();
  CascadedController cascaded_pid(-2.0, 1600.0, 100.0, 200.0, 0.25, env);
  auto pid_controller = [&cascaded_pid](const LanderState &s) -> double {
    return cascaded_pid.action(s);
  };
  log2csv(pid_controller, "pid.csv");

  cout << "Logged to pid.csv, All jobs finished.";
  return 0;
}
