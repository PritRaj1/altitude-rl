#pragma once
#include "env.hpp"

#include <map>
#include <memory>
#include <vector>

using namespace std;

enum class TDtype { QLearning, SARSA };

class Agent {
private:
  double alpha;
  double gamma;
  double epsilon;
  double decay;

  vector<double> action_space; // Discretised thrusts
  vector<double> q_table; // Index by [alt_idx * (NUM_VEL * NUM_ACT) + vel_idx *
                          // NUM_ACT + action_idx]

  // Discretisation
  const int NUM_ACTIONS = 20;
  const double MAX_ALTITUDE = 200;
  const double dALT = 2.0; // Bucket size
  const int NUM_ALT_BUCKETS = static_cast<int>(MAX_ALTITUDE / dALT) + 1;
  const double MIN_VEL = -30.0;
  const double MAX_VEL = 10.0;
  const double dVEL = 1.0; // Bucket size
  const int NUM_VEL_BUCKETS = static_cast<int>((MAX_VEL - MIN_VEL) / dVEL) + 1;

  int get_alt_idx(double altitude) const;
  int get_vel_idx(double velocity) const;
  int get_q_idx(int alt_idx, int vel_idx, int action_idx) const;

public:
  Agent(double alpha, double gamma, double epsilon, double decay,
        const MarsLanderEnv &env);
  ~Agent() = default;

  int choose_action(const LanderState &state, bool eval = false);
  double get_thrust(int action_idx) const;
  void update(TDtype type, const LanderState &state, int action_idx,
              double reward, const LanderState &next_state,
              int next_action_idx);
  void decay_epsilon();
  Agent clone() const;
  void sync_from(const Agent &other);
  void set_q_table(const vector<double> &new_table);
  vector<double> get_q_table() const;
};
