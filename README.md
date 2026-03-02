# ChessLite

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()

School project: 3x3 chess variant with reinforcement learning AI. Supports single-player, multiplayer, and AI vs AI matches. Optional MySQL backend for agent storage.

[How to build](#how-to-build) • [Rules](#game-rules) • [AI](#ai-training) • [Architecture](#architecture)

</div>

<a name="top"></a>

---

## Overview

ChessLite implements a chess variant on a 3x3 board. Two players each control 3 pawns.

Components:
- Game logic and board state management
- SDL3-based UI
- Machine learning agent system
- Optional MySQL backend with network synchronization
- 4 different skins for the pawns

Press **1** during gameplay to show debug overlay displaying:
- Move history of current game (left side)
- All board positions the agent has learned (right side)
- Move evaluations per position 

The application runs offline (no server required) or with optional server connection for persistent agent storage.

---

## Game Rules

3x3 board. 3 pawns per player.

Board layout:
```
  0 1 2
0 [W][W][W]
1 [_][_][_]
2 [B][B][B]
```

Win conditions:
- Move a pawn to the opposite end (row 2 for white, row 0 for black)
- Capture all opponent pawns
- Opponent has no legal moves

Pawn rules:
- Move forward 1 square (if empty)
- Capture diagonally forward

---

## How to build

### Requirements

- Windows 10/11
- MSVC 2022 (C++17)
- Docker Desktop (optional, for database)

### Setup

1. Clone repository:
   ```bash
   git clone https://github.com/Etienne780/ChessLite.git
   cd ChessLite
   ```
   
   Generates Visual Studio project files and compiles.
   ```bash
   build vs2022
   ```
   
3. Start database (optional):
   ```bash
   build db-start
   ```
   
   Starts MySQL container. Application runs without it.

4. Run game or execute from Visual Studio.

### Build Commands

```bash
build help                 # List all commands
build compile              # Generate projects + compile
build vs2022               # Generate project files only

# Database
build db-start             # Start MySQL container
build db-stop              # Stop + delete container
build db-stop-containers   # Stop container, keep volume
build db-status            # Show container status
build db-tables            # List database tables
```

### Database Port Conflict

If port 3306 is busy:

Stop the conflicting process:
```bash
netstat -ano | findstr :3306
taskkill /PID <PID_NUMBER> /F
```
Or
1. Edit `docker-compose.yml`: change `"3306:3306"` to `"3307:3306"`
2. Edit `server/config.otn`: update port in host config
3. Run `build db-start` again

---

## AI Training

The agent learns through self-play using reinforcement learning.

### Algorithm

1. **State Tracking**: Agent stores known board positions with move evaluations
2. **Move Generation**: Generates legal moves for unknown positions
3. **Move Selection**: 
   - Exploration rate decays from 1.0 to 0.0 over ~15 games
   - Picks random move if: `random(0, 1) < exploration_rate`
   - Otherwise picks move with highest evaluation
   - Ties broken randomly
4. **Learning**: After each game, updates move evaluations using reward signal

### Learning Formula

```cpp
float reward = won ? 1.0f : -1.0f;
float reductionAmount = 0.2f;
float currentReward = reward;

for(auto& move : movesPlayed) {
    move.evaluation += currentReward;
    currentReward *= reductionAmount;
}
```

Effect:
- Recent moves get larger reward updates
- Early moves get smaller updates (discount factor 0.2)
- Encourages learning immediate consequences

### Exploration Decay

```
Game 1:  exploration = 1.0
Game 5:  exploration ≈ 0.67
Game 10: exploration ≈ 0.33
Game 15: exploration ≈ 0.0
```

### Notes

- Agents develop position-specific strategies
- Different training opponents produce different playing styles
- Training pairs agents against each other (no supervised learning)
- All data stored locally or synced to database

---

## Architecture

```
┌─────────────────────────────────────┐
│   Application (SDL3)                │
│   - Game logic                      │
│   - UI Rendering                    │
│   - Agent Management                │
└──────────────┬──────────────────────┘
               │
        ┌──────▼──────┐
        │  Offline    │  ← Works standalone
        │   Mode      │
        └──────┬──────┘
               │
        ┌──────▼───────────────────────┐
        │ Network Manager (optional)   │
        │ - INSERT new agents          │
        │ - UPDATE agents after games  │
        │ - DIRTY agents sync only     │
        │ - DELETE agents              │
        └──────┬───────────────────────┘
               │
        ┌──────▼───────────────────────┐
        │  MySQL Database              │
        │  - agents table              │
        │  - board_states table        │
        │  - game_moves table          │
        └──────────────────────────────┘
```

### Database Tables

```
agents
  ├─ id (BIGINT, PK)
  ├─ version (INT)
  ├─ name (VARCHAR)
  ├─ config (LONGTEXT, Base64)
  ├─ matches_played (INT)
  ├─ matches_won (INT)
  ├─ matches_played_white (INT)
  ├─ matches_won_white (INT)
  └─ board_states (FK)
      └─ game_moves (FK)

board_states
  ├─ id (BIGINT, PK)
  ├─ agent_id (BIGINT, FK)
  └─ board_state (VARCHAR)

game_moves
  ├─ id (BIGINT, PK)
  ├─ board_state_id (BIGINT, FK)
  ├─ evaluation (FLOAT)
  ├─ from_x, from_y (INT)
  └─ to_x, to_y (INT)
```
---

## License

MIT License. See LICENSE file.

```
MIT License

Copyright (c) 2024 Etienne780

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```
<div align="center">

[Back to Top](#top) • [License](#license)

Made with SDL3

</div>
