#include "train.hpp"
#include <iostream>

using namespace std;

// Thread-safe queue for rollout experiences
void ThreadSafeReplayBuffer::push_batch(const vector<Experience> &batch,
                                        const atomic<bool> &training_active) {
  unique_lock<mutex> lock(mtx);

  for (const auto &item : batch) {
    cv.wait(lock, [this, &training_active] {
      return buffer.size() < MAX_SIZE ||
             !training_active; // worker sleeps on full buffer
    });

    if (!training_active)
      return;
    buffer.push(item);
  }

  cv.notify_one(); // Wake up global optim as data batch ready
}

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

  cv.notify_all(); // Wake up workers, as space in queue freed
  return true;
}

void ThreadSafeReplayBuffer::deactivate(atomic<bool> &training_active) {
  lock_guard<mutex> lock(mtx);
  training_active = false;
  cv.notify_all();
}

void local_rollout(int worker_id, ThreadSafeReplayBuffer &buffer,
                   Agent &global_agent, const atomic<bool> &training_active,
                   int num_episodes) {
  srand(0 + worker_id);
  MarsLanderEnv env;
  Agent local_agent = global_agent.clone();

  // Pre-allocate temporary cache on this thread
  vector<Experience> local_batch;
  local_batch.reserve(256);

  for (int ep = 0; ep < num_episodes && training_active.load(); ++ep) {
    local_agent.sync_from(global_agent);
    env.reset();
    LanderState state = env.get_state();
    int action = local_agent.choose_action(state);

    while (!env.is_terminal() && training_active.load()) {
      double thrust = local_agent.get_thrust(action);
      env.step(thrust);

      LanderState next_state = env.get_state();
      double reward = env.calculate_reward();
      int next_action = local_agent.choose_action(next_state);

      // Push to private local memory
      Experience item = {state, action, reward, next_state, next_action};
      local_batch.push_back(item);

      state = next_state;
      action = next_action;
    }
    local_agent.decay_epsilon();

    // Lock mutex and push to buffer
    if (!local_batch.empty()) {
      buffer.push_batch(local_batch, training_active);
      local_batch.clear(); // Empty local cache for next ep
    }
  }

  cout << "  [Thread " << worker_id << "] Finished.\n";
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
