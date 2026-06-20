#include "agent.hpp"
#include "env.hpp"

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <limits>

Agent::Agent(double a, double g, double e, const MarsLanderEnv& env)
    : alpha(a), gamma(g), epsilon(e) {
    for (int i = 0; i < NUM_ACTIONS; ++i) {
        double fraction = static_cast<double>(i) / (NUM_ACTIONS - 1);
        action_space.push_back(fraction * env.MAX_THRUST); 
    }
    
    // Contiguous array for Q
    size_t total_elements = NUM_ALT_BUCKETS * NUM_VEL_BUCKETS * NUM_ACTIONS;
    q_table.resize(total_elements, 0.0);
}

int Agent::get_alt_idx(double altitude) const {
    if (altitude < 0.0) altitude = 0.0;
    if (altitude > MAX_ALTITUDE) altitude = MAX_ALTITUDE;
    return static_cast<int>(altitude / dALT);
}

int Agent::get_vel_idx(double velocity) const {
    if (velocity < VEL_MIN) velocity = VEL_MIN;
    if (velocity > VEL_MAX) velocity = VEL_MAX;
    return static_cast<int>((velocity - VEL_MIN) / dVEL);
}

inline int Agent::get_q_index(int alt_idx, int vel_idx, int action_idx) const {
    return (alt_idx * NUM_VEL_BUCKETS * NUM_ACTIONS) + (vel_idx * NUM_ACTIONS) + action_idx;
}

double Agent::get_thrust(int action_idx) const {
    return action_space[action_idx];
}

// Epsilon-greedy choice
int Agent::choose_action(const LanderState& state) {
    if ((static_cast<double>(rand()) / RAND_MAX) < epsilon) {
        return rand() % num_actions; // explore
    }

    // Exploit
    int alt_idx = get_alt_idx(state.altitude);
    int vel_idx = get_vel_index(state.velocity);

    int best_action = 0;
    double max_q = -numeric_limits<double>::infinity();
    int base_offset = get_q_index(alt_idx, vel_idx, 0);
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        double q_val = q_table[base_offset + a];
        if (q_val > max_q) {
            max_q = q_val;
            best_action = a;
        }
    }
    return best_action;
}

void Agent::update(TDType type, const LanderState& state, int action_idx, double reward, const LanderState& next_state, int next_action_idx) {
    int alt_idx = get_alt_index(state.altitude);
    int vel_idx = get_vel_index(state.velocity);
    int current_q_idx = get_q_index(alt_idx, vel_idx, action_idx);

    int next_alt_idx = get_alt_index(next_state.altitude);
    int next_vel_idx = get_vel_index(next_state.velocity);
    int next_base_offset = get_q_index(next_alt_idx, next_vel_idx, 0);

    double target_future_q = 0.0;

    if (type == TDType::QLearning) {
        double max_future_q = -numeric_limits<double>::infinity();
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            max_future_q = max(max_future_q, q_table[next_base_offset + a]);
        }
        target_future_q = max_future_q;
    } else {
        target_future_q = q_table[next_base_offset + next_action_idx];
    }

    // Bellman update
    q_table[current_q_idx] += alpha * (reward + gamma * target_future_q - q_table[current_q_idx]);
}

void Agent::decay_epsilon(double factor) {
    epsilon = max(0.01, epsilon * factor);
}
