## MDP definition

To formulate a Tic-Tac-Toe Markov Decision Process (MDP), we define it using the tuple $(S,A,P,R,\gamma)$.

### 1. State Space (S)

A state $s \in S$ is defined:

$$
s=(x_\text{board},o_\text{board},\text{player})
$$

Where:

- $x_\text{board} \in \{ 0,1 \}^9: \;$ A bitvector (or integer mask) representing X's placements on the 3×3 grid
- $_\text{board} \in \{ 0,1 \}^9: \;$ A bitvector representing O's placements
- $\text{player} \in \{ X, O \}: \;$ Whose turn it is

Constraints & Terminal States:

1. Validity Constraint: $\; x_\text{board} \cdot o_\text{board}=0 \;$ (no overlapping pieces)
2. Terminal States ($S_T \subset S$): $\;$ a state is terminal if
    - $x_\text{board} \;$ or $\; o_\text{board} \;$ matches any of the 8 winning line masks (3 rows, 3 columns, 2 diagonals)
    - $\text{popcount}(x_\text{board})+\text{popcount}(o_\text{board})=9 \;$ (a draw/full board).

### 2. Action Space (A)

An action $a$ represents choosing an index on the grid to place the current player's token.

$$
A=\{0,1,2,3,4,5,6,7,8\}
$$

The set of legal actions available from a specific state $s$, denoted A(s), is the set of empty spaces:

$$
A(s)=\{i \in A \vert x_\text{board}\left[i\right]=0 \; \cap \; o_\text{board}\left[i\right]=0\}
$$

If $s \in S_T$, $A(s)=\emptyset$.

### 3. Transition Function/Probability (P)

Tic-Tac-Toe has deterministic rules, but how you model the transition function depends on whether you are looking at it from a single-player or two-player perspective.

#### A: Single-Agent MDP (this repo)

For a single agent (e.g., player $X$) against a fixed or stochastic opponent, the opponent's strategy is embedded into the transition function.

Let $a \in A(s)$ be the action chosen by player $X$ at state $s=(x_\text{board},o_\text{board},X)$.

1. Intermediate state after $X$'s move: $s^\prime=(x_\text{board} \cup \{a\}, o_\text{board},O)$
2. If $s^\prime$ is terminal, $P(s^\prime \vert s,a)=1$
3. If $s^\prime$ is not terminal, the opponent ($O$) chooses an action $a_O \in A(s^\prime)$ according to their policy $\pi_O(a_O \vert s^\prime)$
4. The final next state transitions to $s^{\prime \prime}=(x_\text{board}\cup\{a\},o_\text{board}\cup\{a_O\},X)$.

Thus, the transition probability to the next agent state is:
$$
P(s^{\prime \prime} \vert s,a)=\pi_O(a_O\vert s^\prime)
$$

#### B: Zero-Sum Game (MARL)

If you treat the MDP as an environment where the agent swaps every turn (the $\text{player}$ variable changes explicitly), the transition is deterministic:

$$
P(s^\prime \mid s, a) =
\begin{cases}
1 & \text{if player} = X \text{ and } s^\prime = (x_\text{board} \cup \{a\}, o_\text{board}, O) \\
1 & \text{if player} = O \text{ and } s^\prime = (x_\text{board}, o_\text{board} \cup \{a\}, X) \\
0 & \text{otherwise}
\end{cases}
$$

### 4. Reward Function ($R$)

The reward function is defined at the terminal states. Assuming we are modeling the MDP from **Player $X$**'s perspective:

$$
R(s, a, s^\prime) =
\begin{cases}
+1 & \text{if } s^\prime \text{ is a win for } X \\
-1 & \text{if } s^\prime \text{ is a win for } O \\
0 & \text{if } s^\prime \text{ is a draw or a non-terminal state}
\end{cases}
$$

To penalize taking longer to win, a small step penalty $\epsilon$ can be added (e.g., $R(s,a,s^\prime) = -\epsilon$ for non-terminal moves).

### 5. Discount Factor ($\gamma$)

- **$\gamma = 1$**: Since Tic-Tac-Toe is a guaranteed episodic game that terminates in at most 9 steps, discounting is not necessary.
- **$\gamma < 1$ (e.g., $0.99$)**: Used only to incentivise the agent to choose the shortest path to a win or the longest path to a loss.
