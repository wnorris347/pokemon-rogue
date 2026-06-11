# Pokemon Rogue (poke327)

A terminal-based Pokémon roguelike that began as a semester-long project for Jeremy Sheaffer's COM S 327 course at Iowa State University (Fall 2023). Explore a procedurally generated overworld, battle trainers and wild Pokémon, catch new team members, and level up your party through experience and evolution.

## Features

### Overworld

- **Procedural maps** — Each map is generated with varied terrain (mountains, forests, water, grass, paths, gates, and more).
- **Large world** — Travel across a 401×401 grid of interconnected maps via gates at map edges.
- **Turn-based movement** — Player and NPCs act on a turn queue ordered by movement cost across terrain.
- **NPC trainers** — Hikers, rivals, swimmers, and other trainers with distinct movement behaviors (pace, wander, sentry, explore, swim).
- **Line of sight** — Trainers detect and challenge the player when they have a clear view.
- **Services** — Pokémon Centers (`C`) heal your team and restore PP; Pokemarts (`M`) restock items.
- **Wild encounters** — Walking in tall grass (`:`) has a 10% chance to trigger a wild battle.

### Pokémon & Battles

- **Starter selection** — Choose one of three randomly generated Pokémon at the start of a run.
- **Trainer and wild battles** — Turn-based combat with Fight, Bag, Run, and Pokémon swap options.
- **Type system** — Damage uses type effectiveness, STAB bonuses, and critical hits.
- **Moves & PP** — Pokémon learn moves from the database; out-of-PP Pokémon use Struggle.
- **Items** — Potions, Revives, and Poké Balls (usable from the bag during battle).
- **Catching** — Wild Pokémon can be caught with Poké Balls (party limit of six).

### Experience, Leveling & Evolution

- **Experience gain** — Defeating enemy Pokémon awards XP to your team using the **Generation IX** formula.
- **Level-up** — Pokémon gain levels, updated stats, and may learn new moves (with prompts to forget a move when the move set is full).
- **Evolution** — Level-based evolution is supported (trade, item, friendship, and move-based evolutions are not).
- **Post-battle evolution** — Pokémon evolve after battle if they leveled up and meet their evolution level.

### Quality of Life

- **Skippable text** — Most gameplay-interrupting messages can be advanced by pressing any key.
- **Optional seed** — Reproducible runs via `-s` / `--seed`.
- **Debug movement** — `f` flies to any map; `p` teleports within the current map; `t` lists trainers on the map.

> **Note:** Unlike Generation IX games, Pokémon do **not** gain experience when a wild Pokémon is caught.

## Requirements

- **Linux** (or WSL) with a POSIX environment
- `gcc` and `g++`
- `make`
- `libncurses` development headers (e.g. `libncurses-dev` on Debian/Ubuntu)
- Pokémon CSV data in `data/csv/` (included in this repository; see [Data setup](#data-setup))

## Data setup

At startup, the game loads Pokémon data from CSV files in `data/csv/` relative to the project root. No symlinks or extra configuration are required — run `poke327` from the project directory after building (see [Running](#running)).

## Building

From the project root:

```bash
make
```

This compiles all source files and links the `poke327` binary. To remove build artifacts:

```bash
make clean
```

## Running

From the project root:

```bash
./poke327
```

To use a specific random seed:

```bash
./poke327 -s 12345
./poke327 --seed 12345
```

The seed in use is printed at startup (`Using seed: ...`).

## Controls

| Key | Action |
|-----|--------|
| Numpad `1`–`9`, `h`/`j`/`k`/`l`, arrow keys | Move (vi-style and numpad layouts supported) |
| `>` | Interact (Poké Mart / Pokémon Center when standing on `M` or `C`) |
| `t` | List trainers on the current map |
| `p` | Teleport to a random location on the current map |
| `f` | Fly to another map (enter coordinates) |
| `Q` | Quit |

During battle, follow on-screen prompts for Fight, Bag, Run, Pokémon, move selection, and item use.

## Project structure

| Path | Description |
|------|-------------|
| `poke327.cpp` | Map generation, world simulation, main game loop |
| `battle.cpp` | Battle logic, XP, leveling, evolution, damage calculation |
| `character.cpp` | NPC movement, pathfinding, line of sight |
| `io.cpp` | ncurses display, input, shops, battle UI |
| `db_parse.cpp` | Loads Pokémon/move/species data from CSV files |
| `heap.c` | Binary heap for turn queue and pathfinding |
| `data/csv/` | Pokémon database CSV files |
| `Makefile` | Build configuration |

## Origins & attribution

poke327 started as the COM S 327 semester project at Iowa State University (Fall 2023), taught by Dr. Jeremy Sheaffer.

**Course-provided code** includes map generation, seeding, and helper utilities such as the heap and pair modules.

**Original work** in this codebase includes battle mechanics, movement, leveling and evolution systems, and the majority of the UI design.

Pokémon data is derived from the course pokedex CSV dataset.
