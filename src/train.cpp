#include "train.hpp"
#include <iostream>

using namespace std;

// Thread-safe queue for rollout experiences
void ThreadSafeReplayBuffer::push(const Experience &item) {
  lock_guard<mutex> lock(mtx);
  if (buffer.size() >= MAX_SIZE) {
    buffer.pop(); // rm oldest
  }
  buffer.push(item);
  cv.notify_one();
};

bool ThreadSafeReplayBuffer::pop(Experience &item,
                                 const atomic<bool> &training_active) {
  unique_lock<mutex> lock(mtx);

  // Thread sleeps until data ready or training stopped
  cv.wait(lock, [this, &training_active] {
    return !buffer.empty() || !training_active;
  });

  if (buffer.empty() && !training_active) {
    return false;
  }

  item = buffer.front();
  buffer.pop();
  return true;
};

void local_rollout(int worker_id, ThreadSafeReplayBuffer &buffer,
                   const atomic<bool> &training_active, int num_episodes) {
  srand(0 + worker_id);
  MarsLanderEnv env;
  Agent local_agent(0.01, 1.0, 1.0, env);

  for (int ep = 0; ep < num_episodes && training_active.load(); ++ep) {
    env.reset();
    LanderState state = env.get_state();
    int action = local_agent.choose_action(state);

    while (!env.is_terminal() && training_active.load()) {
      double thrust = local_agent.get_thrust(action);
      env.step(thrust);

      LanderState next_state = env.get_state();
      double reward = env.calculate_reward(thrust);
      int next_action = local_agent.choose_action(next_state);

      Experience item{state, action, reward, next_state, next_action};
      buffer.push(item);

      state = next_state;
      action = next_action;
    }
    local_agent.decay_epsilon(0.999);
  }

  cout << "  [Thread " << worker_id << "] Finished.\n";
  return;
}

void global_optim(Agent &global_agent, ThreadSafeReplayBuffer &buffer,
                  const atomic<bool> &training_active, TDtype type) {
  Experience item;
  long long global_updates_acc = 0;

  while (buffer.pop(item, training_active)) {
    global_agent.update(type, item.state, item.action, item.reward,
                        item.next_state, item.next_action);
    global_updates_acc++;
  }

  cout << "  [Optim thread] Finished. Total learning steps: "
       << global_updates_acc << "\n";
  return;
}
