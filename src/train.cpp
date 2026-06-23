#include "train.hpp"
#include <iostream>

using namespace std;

// Thread-safe queue for rollout experiences
void ThreadSafeReplayBuffer::push(const Experience &exp) {
  lock_guard<mutex> lock(mtx);
  if (buffer.size() >= MAX_SIZE) {
    buffer.pop(); // rm oldest
  }
  buffer.push(exp);
  cv.notify_one();
};

bool ThreadSafeReplayBuffer::pop(Experience &exp,
                                 const atomic<bool> &training_active) {
  unique_lock<mutex> lock(mtx);

  // Thread sleeps until data ready or training stopped
  cv.wait(lock, [this, &training_active] {
    return !buffer.empty() || !training_active;
  });

  if (buffer.empty() && !training_active) {
    return false;
  }

  exp = buffer.front();
  buffer.pop();
  return true;
};
