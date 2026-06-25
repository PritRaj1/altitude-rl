#pragma once

#include "agent.hpp"
#include "env.hpp"
#include "utils.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

struct Experience {
  LanderState state;
  int action;
  double reward;
  LanderState next_state;
  int next_action;
};

class ThreadSafeReplayBuffer {
private:
  std::queue<Experience> buffer;
  std::mutex mtx;
  std::condition_variable cv;
  static constexpr size_t MAX_SIZE = 5000;

public:
  void push_batch(const std::vector<Experience> &batch,
                  const std::atomic<bool> &training_active);
  bool pop(Experience &exp, const std::atomic<bool> &training_active);
  void deactivate(std::atomic<bool> &training_active);
};
void local_rollout(int worker_id, ThreadSafeReplayBuffer &buffer,
                   Agent &global_agent,
                   const std::atomic<bool> &training_active, int num_episodes);
void global_optim(Agent &global_agent, ThreadSafeReplayBuffer &buffer,
                  const std::atomic<bool> &training_active, TDtype type);
