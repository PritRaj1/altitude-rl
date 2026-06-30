#include "agent.hpp"
#include "env.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <shared_mutex>

// Init from params
Agent::Agent(double a, double g, double e, double d, const MarsLanderEnv &env)
    : alpha(a), gamma(g), epsilon(e), decay(d) {
  for (int i = 0; i < NUM_ACTIONS; ++i) {
    double fraction = static_cast<double>(i) / (NUM_ACTIONS - 1);
    action_space.push_back(fraction * env.MAX_THRUST);
  }

  // Contiguous array for Q
  size_t total_elements = NUM_ALT_BUCKETS * NUM_VEL_BUCKETS * NUM_ACTIONS;
  q_table.resize(total_elements, 0.0);
}

// thread-safe copy
Agent::Agent(const Agent &other)
    : alpha(other.alpha), gamma(other.gamma), epsilon(other.epsilon),
      decay(other.decay), action_space(other.action_space) {
  std::shared_lock<std::shared_mutex> lock(other.agent_mtx);
  q_table = other.q_table;
}

int Agent::get_alt_idx(double altitude) const {
  if (altitude < 0.0)
    altitude = 0.0;
  if (altitude > MAX_ALTITUDE)
    altitude = MAX_ALTITUDE;
  return static_cast<int>(altitude / dALT);
}

int Agent::get_vel_idx(double velocity) const {
  if (velocity < MIN_VEL)
    velocity = MIN_VEL;
  if (velocity > MAX_VEL)
    velocity = MAX_VEL;
  return static_cast<int>((velocity - MIN_VEL) / dVEL);
}

inline int Agent::get_q_idx(int alt_idx, int vel_idx, int action_idx) const {
  return (alt_idx * NUM_VEL_BUCKETS * NUM_ACTIONS) + (vel_idx * NUM_ACTIONS) +
         action_idx;
}

double Agent::get_thrust(int action_idx) const {
  return action_space[action_idx];
}

// Epsilon-greedy choice
int Agent::choose_action(const LanderState &state, bool eval) {
  if (!eval && (static_cast<double>(rand()) / RAND_MAX) < epsilon) {
    return rand() % NUM_ACTIONS; // explore
  }

  // Exploit
  int alt_idx = get_alt_idx(state.altitude);
  int vel_idx = get_vel_idx(state.velocity);

  int best_action = 0;
  double max_q = -numeric_limits<double>::infinity();
  int base_offset = get_q_idx(alt_idx, vel_idx, 0);
  for (int a = 0; a < NUM_ACTIONS; ++a) {
    double q_val = q_table[base_offset + a];
    if (q_val > max_q) {
      max_q = q_val;
      best_action = a;
    }
  }
  return best_action;
}

void Agent::update(TDtype type, const LanderState &state, int action_idx,
                   double reward, const LanderState &next_state,
                   int next_action_idx) {
  std::unique_lock<std::shared_mutex> write_lock(agent_mtx);

  int alt_idx = get_alt_idx(state.altitude);
  int vel_idx = get_vel_idx(state.velocity);
  int current_q_idx = get_q_idx(alt_idx, vel_idx, action_idx);

  int next_alt_idx = get_alt_idx(next_state.altitude);
  int next_vel_idx = get_vel_idx(next_state.velocity);
  int next_base_offset = get_q_idx(next_alt_idx, next_vel_idx, 0);

  double target_future_q = 0.0;

  if (type == TDtype::QLearning) {
    double max_future_q = -numeric_limits<double>::infinity();
    for (int a = 0; a < NUM_ACTIONS; ++a) {
      max_future_q = max(max_future_q, q_table[next_base_offset + a]);
    }
    target_future_q = max_future_q;
  } else {
    target_future_q = q_table[next_base_offset + next_action_idx];
  }

  // Bellman update
  q_table[current_q_idx] +=
      alpha * (reward + gamma * target_future_q - q_table[current_q_idx]);
}

void Agent::decay_epsilon() { epsilon = max(0.1, epsilon * decay); }

Agent Agent::clone() const { return Agent(*this); }

void Agent::sync_from(const Agent &other) {
  std::shared_lock<std::shared_mutex> read_lock(other.agent_mtx);
  this->q_table = other.q_table;
  this->epsilon = other.epsilon;
}

void Agent::set_q_table(const vector<double> &new_table) {
  q_table = new_table;
}

vector<double> Agent::get_q_table() const { return q_table; }
