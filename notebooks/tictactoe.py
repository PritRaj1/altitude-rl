from typing import List, Tuple, Set, Dict, Optional
from collections import deque
import random
import numpy as np

BITBOARD_WIN_PATTERNS = [
    0b111000000,  # Top row
    0b000111000,  # Middle row
    0b000000111,  # Bottom row
    0b100100100,  # Left column
    0b010010010,  # Middle column
    0b001001001,  # Right column
    0b100010001,  # Diagonal
    0b001010100,  # Other diagonal
]


class TicTacToeMDP:
    def __init__(self):
        self.all_states: Set[Tuple[int, int, int]] = set()
        self._discover_state_space()

    @staticmethod
    def is_terminal(state: Tuple[int, int, int]) -> bool:
        x_board, o_board, _ = state
        for pattern in BITBOARD_WIN_PATTERNS:
            if (x_board & pattern) == pattern or (o_board & pattern) == pattern:
                return True

        return (x_board | o_board) == 0b111111111  # Draw

    @staticmethod
    def get_reward_for_x(state: Tuple[int, int, int]) -> float:
        if not TicTacToeMDP.is_terminal(state):
            return 0.0

        x_board, o_board, _ = state
        for pattern in BITBOARD_WIN_PATTERNS:
            if (x_board & pattern) == pattern:
                return 1.0
            if (o_board & pattern) == pattern:
                return -1.0

        return 0.0  # Draw

    @staticmethod
    def get_legal_actions(state: Tuple[int, int, int]) -> List[Tuple[int, int]]:
        if TicTacToeMDP.is_terminal(state):
            return []

        x_board, o_board, _ = state
        occupied = x_board | o_board
        empty = ~occupied & 0b111111111
        return [divmod(i, 3) for i in range(9) if empty & (1 << (8 - i))]

    @staticmethod
    def step(
        state: Tuple[int, int, int], action: Tuple[int, int]
    ) -> Tuple[int, int, int]:
        """State transition for a single move."""
        x_board, o_board, player_turn = state
        bit = 8 - (action[0] * 3 + action[1])
        if player_turn == 1:
            return (x_board | (1 << bit), o_board, 0)
        else:
            return (x_board, o_board | (1 << bit), 1)

    @staticmethod
    def render_state_tuple(state: Tuple[int, int, int], v_dict: Optional[Dict] = None):
        """Renders DP state tuple into a visual text board with optional value lookup."""
        x_board, o_board, turn = state
        board = []
        for i in range(9):
            mask = 1 << (8 - i)
            if x_board & mask:
                board.append("X")
            elif o_board & mask:
                board.append("O")
            else:
                board.append(" ")

        rows = [
            f" {board[0]} | {board[1]} | {board[2]} ",
            "---+---+---",
            f" {board[3]} | {board[4]} | {board[5]} ",
            "---+---+---",
            f" {board[6]} | {board[7]} | {board[8]} ",
        ]
        print("\n".join(rows))
        if v_dict is not None:
            print(f"Current Value: {v_dict.get(state, 0.0):.4f}")
        print("-" * 15)

    def play_simulation_game(
        self, policy: Dict, v_dict: Optional[Dict] = None, human_player: bool = False
    ):
        """
        If human_player=True, you play as O against optimal X policy.
        If human_player=False, X plays against uniform random environment opponent.
        """
        state = (0, 0, 1)
        print("Initial Board:")
        self.render_state_tuple(state, v_dict)

        while not self.is_terminal(state):
            x_board, o_board, turn = state

            if turn == 1:
                action = policy[state]
                print(f"Optimal X plays: {action}")
                state = self.step(state, action)
            else:
                legal_actions = self.get_legal_actions(state)
                if human_player:
                    print(f"Legal moves available: {legal_actions}")
                    while True:
                        try:
                            inp = input("Enter move as row,col (e.g., 1,1): ")
                            row, col = map(int, inp.split(","))
                            if (row, col) in legal_actions:
                                action = (row, col)
                                break
                            print("Space is occupied or invalid!")

                        except ValueError:
                            print("Invalid format. Use row,col integers.")
                else:
                    action = random.choice(legal_actions)
                    print(f"Stochastic Opponent O plays random action: {action}")

                state = self.step(state, action)

            self.render_state_tuple(state, v_dict)

        reward = self.get_reward_for_x(state)
        if reward == 1.0:
            print("Game Over: Optimal Agent X Wins!")
        elif reward == -1.0:
            print("Game Over: Stochastic Opponent O Wins!")
        else:
            print("Game Over: It's a Draw!")

    def _discover_state_space(self):
        """Generates all reachable states for X turn or terminal."""
        start_state = (0b000000000, 0b000000000, 1)
        queue = deque([start_state])
        self.all_states.add(start_state)

        while queue:
            curr = queue.popleft()

            if self.is_terminal(curr):
                continue

            if curr[2] == 1:  # X turn
                for act in self.get_legal_actions(curr):
                    next_state = self.step(curr, act)

                    # BFS: resolve all branches to find next X states
                    if (
                        not self.is_terminal(next_state) and next_state[2] == 0
                    ):  # Player O
                        for o_act in self.get_legal_actions(next_state):
                            after_o = self.step(next_state, o_act)
                            if after_o not in self.all_states:
                                self.all_states.add(after_o)
                                queue.append(after_o)
                    else:
                        if next_state not in self.all_states:
                            self.all_states.add(next_state)
                            queue.append(next_state)

    def get_all_states(self) -> List[Tuple[int, int, int]]:
        """Returns only states where X makes a decision, plus final terminal states."""
        return [s for s in self.all_states if s[2] == 1 or self.is_terminal(s)]

    def get_transitions(
        self, state: Tuple[int, int, int], action: Tuple[int, int]
    ) -> List[Tuple[float, Tuple[int, int, int], float, bool]]:
        if self.is_terminal(state):
            return []

        next_state = self.step(state, action)
        if self.is_terminal(next_state):
            return [(1.0, next_state, self.get_reward_for_x(next_state), True)]

        o_actions = self.get_legal_actions(next_state)
        prob = 1.0 / len(o_actions)
        transitions = []

        for o_act in o_actions:
            after_o = self.step(next_state, o_act)
            is_term = self.is_terminal(after_o)
            reward = self.get_reward_for_x(after_o)
            transitions.append((prob, after_o, reward, is_term))

        return transitions

    def get_q_value(
        self, Q: Dict, state: Tuple[int, int, int], action: Tuple[int, int]
    ) -> float:
        """Extracts value from Tabular Q-Table, defaulting to 0.0."""
        return Q.get((state, action), 0.0)

    def choose_epsilon_greedy_action(
        self, Q: Dict, state: Tuple[int, int, int], epsilon: float
    ) -> Tuple[int, int]:
        legal_actions = self.get_legal_actions(state)
        if not legal_actions:
            return None

        if random.random() < epsilon:
            return random.choice(legal_actions)
        else:
            best_actions = []
            max_q = -float("inf")
            for a in legal_actions:
                q_val = self.get_q_value(Q, state, a)
                if q_val > max_q:
                    max_q = q_val
                    best_actions = [a]

                elif q_val == max_q:
                    best_actions.append(a)

            # Tie-break randomly among identical maximal actions
            return random.choice(best_actions)

    def sample_environment_step(
        self, state: Tuple[int, int, int], action: Tuple[int, int]
    ) -> Tuple[Tuple[int, int, int], float, bool]:
        """Sample transition from environment"""
        transitions = self.get_transitions(state, action)
        if not transitions:
            return state, self.get_reward_for_x(state), True

        probabilities = [t[0] for t in transitions]

        # Choose one trajectory index based on transition probabilities
        sampled_idx = np.random.choice(len(transitions), p=probabilities)
        _, next_state, reward, is_terminal = transitions[sampled_idx]
        return next_state, reward, is_terminal
