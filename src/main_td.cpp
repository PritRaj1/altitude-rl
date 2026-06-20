#include "env.hpp"
#include "agent.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void train_agent(TDtype type, const string& filename, int num_episodes = 25000) {
    ofstream csv_file(filename);
    csv_file << "episode,total_reward,final_velocity\n";

    MarsLanderEnv env;
    Agent agent(0.05, 0.99, 1.0, env);

    string run_name = (type == TDtype::QLearning) ? "Q-Learning" : "SARSA";
    cout << "Starting training: " << run_name << "\n";

    for (int ep = 0; ep < num_episodes; ++ep) {
        double cumulative_reward = 0.0;
        LanderState state = env.get_state();
        int action = agent.choose_action(state);

        while (!env.is_terminal()) {
            double thrust = agent.get_thrust(action);
            env.step(thrust);
            LanderState next_state = env.get_state();
            double reward = env.calculate_reward(thrust);
            cumulative_reward += reward;

            int next_action = agent.choose_action(next_state);
            agent.update(type, state, action, reward, next_state, next_action);
            state = next_state;
            action = next_action;
        }

        agent.decay_epsilon(0.998);
        csv_file << ep << "," << cumulative_reward << "," << env.get_state().velocity << "\n";
        if (ep % 5000 == 0 && ep > 0) {
            cout << "  -> [" << run_name << "] Step: " << ep << "/" << num_episodes << " complete.\n";
        }
    }
    cout << "Finished " << run_name << ". Output to " << filename << "\n\n";
}

int main() {
    srand(1337); 
    train_agent(TDtype::QLearning, "q_learning_curves.csv");
    train_agent(TDtype::SARSA, "sarsa_learning_curves.csv");
    return 0;
}
