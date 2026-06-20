#include "env.hpp"
#include "agent.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void train_agent(TDtype type, const string& filename, int num_episodes = 25000) {
    ofstream csv_file(filename);
    csv_file << "episode,reward,velocity\n";

    MarsLanderEnv env;
    Agent agent(0.05, 0.99, 1.0, env);

    string run_name = (type == TDtype::QLearning) ? "Q-Learning" : "SARSA";
    cout << "Starting training: " << run_name << "\n";

    // Periodically sample trials
    const int EVAL_INTERVAL = 500;
    const int EVAL_SAMPLES = 100;

    for (int ep = 0; ep < num_episodes; ++ep) {
        
        // Eval
        if (ep % EVAL_INTERVAL == 0) {
            for (int sim = 0; sim < EVAL_SAMPLES; ++sim) {
                MarsLanderEnv eval_env; 
                eval_env.reset();
                double eval_cum_reward = 0.0;
                
                LanderState eval_state = eval_env.get_state();
                while (!eval_env.is_terminal()) {
                    int action = agent.choose_action(eval_state, true); 
                    double thrust = agent.get_thrust(action);
                    eval_env.step(thrust);
                    
                    eval_cum_reward += eval_env.calculate_reward(thrust);
                    eval_state = eval_env.get_state();
                }
                csv_file << ep << "," << eval_cum_reward << "," << eval_env.get_state().velocity << "\n";
            }
        }

        env.reset();
        LanderState state = env.get_state();
        int action = agent.choose_action(state); // Defaults to evaluate=false
        while (!env.is_terminal()) {
            double thrust = agent.get_thrust(action);
            env.step(thrust);
            LanderState next_state = env.get_state();
            double reward = env.calculate_reward(thrust);

            int next_action = agent.choose_action(next_state);
            agent.update(type, state, action, reward, next_state, next_action);
            state = next_state;
            action = next_action;
        }

        agent.decay_epsilon(0.998);
        
        if (ep % 5000 == 0 && ep > 0) {
            cout << "  -> [" << run_name << "] Episode: " << ep << "/" << num_episodes << " complete.\n";
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
