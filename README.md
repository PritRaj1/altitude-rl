# altitude-rl

SARSA/Q-learning to guide a mars lander from 100 meters altitude to a soft touchdown.

## Multithreading

The approach is presented [here](https://arxiv.org/abs/1803.00933):

### Producer

Multiple threads run environments with local agents choosing actions, pushing experiences to a thread-safe queue:

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
// train.hpp
class ThreadSafeReplayBuffer {
private:
  std::queue<Experience> buffer;
  std::mutex mtx; // protect queue
  std::condition_variable cv; // Freeze global optimizer until workers push experiences
  static constexpr size_t MAX_SIZE = 500;

public:
  void push_batch(const vector<Experience> &exp, std::atomic<bool> &training_active); // push to queue in episodic batches to reduce amount of sync required
  bool pop(Experience &exp, const std::atomic<bool> &training_active);
  void deactivate(std::atomic<bool> &training_active); // prevents lost wake up race condition by waking up any stalled workers
};
```

To prevent race conditions between local agent syncing and global agent updating, `std::shared_mutex` (C++ 20) is used:

- `std::shared_lock` allows threads to read the global Q-table simultaneously without blocking each other
- `std::unique_lock` locks access to the optimizer during weight updates, so no worker is reading a partially updated table
