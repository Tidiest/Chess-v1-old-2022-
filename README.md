# Chess AI

A chess application written in **C++** using **SFML 2.5.1**. It includes a custom chess engine with move generation, position evaluation and a search algorithm based on **Negamax with Alpha-Beta pruning**.

The project was created in **2022** as a personal programming experiment while I was still learning and exploring C++. It is an older, playable version of the project and is preserved as a demonstration of that work.

## Try it

A prebuilt Windows executable is included in the repository:

**[Download Chess.exe](https://github.com/Tidiest/Chess-v1-old-2022-/raw/refs/heads/main/Chess.exe)**

Download the executable and run it on Windows. Keep the `sprites/` and `sounds/` folders next to the executable, as the application uses these files.

The player controls White and plays against the built-in AI. Use the **left mouse button** to select and move pieces.

> The executable is provided as a convenience. Compatibility may depend on the Windows environment and required runtime components.

## Features

- Graphical chessboard and pieces rendered with SFML
- Human vs. AI gameplay
- Generation and validation of legal moves
- Board state management, including making and undoing moves
- Special moves: castling, en passant and pawn promotion
- Position evaluation using material values and piece-square tables
- Endgame-related positional evaluation
- Negamax search with Alpha-Beta pruning
- Move ordering to improve search efficiency
- Sound effects for moves and other chess events

## How the AI works

The engine searches possible move sequences using **Negamax with Alpha-Beta pruning**. A separate evaluation function estimates the strength of a position based on material balance, piece-square tables and positional factors, including king positioning.

Candidate moves are ordered using scores that prioritize moves such as captures and promotions. This helps the search examine promising moves earlier.

The search depth is configured in the application. The engine has not been independently benchmarked against established chess engines, so no official Elo rating is claimed.

## Source code overview

| File | Responsibility |
|---|---|
| `Chess.cpp` | Application entry point, SFML window, input handling and main game loop. Connects the interface with the chess engine. |
| `Board.h / Board.cpp` | Board representation and state management. Handles making and undoing moves, as well as special-move state. |
| `MoveGenerator.h / MoveGenerator.cpp` | Generates pseudo-legal and legal moves for the chess pieces and handles special move generation. |
| `Search.h / Search.cpp` | AI search algorithms, including Negamax and Alpha-Beta pruning. Also contains move-generation testing code. |
| `Evaluation.h / Evaluation.cpp` | Evaluates positions using material, piece-square tables and positional/endgame factors. |
| `MoveOrdering.h / MoveOrdering.cpp` | Assigns scores to candidate moves and orders them for the search. |
| `PieceBoard.h / PieceBoard.cpp` | Piece-square table data used by position evaluation. |
| `BoardDrawer.h / BoardDrawer.cpp` | Draws the chessboard and pieces using SFML. |
| `BoardHelper.h / BoardHelper.cpp` | Helper utilities for board coordinates and related operations. |
| `Chess.vcxproj` | Visual Studio project configuration. |
| `Chess.vcxproj.filters` | Visual Studio source-file filters. |
| `Chess.vcxproj.user` | Local Visual Studio user settings. |
| `NumberPositions.txt` | Reference position-count data for move-generation testing. |
| `MyNumPositions.txt` | Position-count data used/generated during move-generation testing. |

### Assets

- `sprites/` — chess piece and board graphics.
- `sounds/` — sound effects for moves, captures, castling, checks and promotions.

## Build from source

The project was originally configured for **Visual Studio** and **SFML 2.5.1**.

To compile it on another machine:

1. Install Visual Studio with the C++ desktop development workload.
2. Install a compatible SFML version and architecture.
3. Update the SFML include and library paths in the project properties. The checked-in project file contains paths from the original development environment.
4. Open `Chess.vcxproj` and build the project.

The included executable is the easiest way to try the application without setting up the development environment.

---

## Author

**Tidiest**

Originally created in **2022** as an experimental personal project while learning C++ and exploring chess programming.
