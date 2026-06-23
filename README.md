# altitude-sim

Concurrent C++ SARSA/Q-learning to guide a mars lander from 100 meters alititude to a soft touchdown.

## Multithreading approach

The approach is presented [here](https://arxiv.org/abs/1803.00933):

### Producer

Runs trajectories within local environment with local agent choosing actions, pushing experiences to a thread-safe queue:

```c++
// train.cpp
void local_rollout(
    int worker_id,
    ThreadSafeReplayBuffer &buffer, // thread-safe queue to push to
    const atomic<bool> &training_active, // thread-safe bool
    int num_episodes
)
```

### Consumer

Single global optimizer for Bellman updates. Sequentially pops experiences from thread-safe queue to update global Q-table:

```c++
// train.cpp
void global_optim(
    Agent &global_agent, // Master agent
    ThreadSafeReplayBuffer &buffer, // thread-safe queue to pop from
    const atomic<bool> &training_active, // thread-safe bool
    TDtype type // Q-learning or SARSA
)
```

### Thread guards

Threads are locked with `std::mutex` & `std::lock_guard` / `std::unique_lock` so only one worker can modify the shared queue's pointers at any instant:

```c++
// tain.hpp
class ThreadSafeReplayBuffer {
private:
  std::queue<Experience> buffer;
  std::mutex mtx;
  std::condition_variable cv; // Freeze global optimizer until workers push experiences
  static constexpr size_t MAX_SIZE = 500;

public:
  void push(const Experience &exp);
  bool pop(Experience &exp, const std::atomic<bool> &training_active);
};
```
