# Molecular Tile Simulation (C++)

## Overview

This project is a grid-based molecular reaction simulator that models how neighboring reactants interact over time.

Each tile is represented as a 2D matrix where:

- Each cell contains a reactant
- Neighboring cells may react
- Reactions occur probabilistically based on defined rates

As reactions occur, the tile state and reactant concentrations update dynamically.

---

## Structure

The system is organized into modular components:

- **Tile** – Maintains simulation state (grid, reactions, concentrations)
- **TileIO** – Handles file input/output and parsing
- **Sim** – Advances the simulation using propensity-based reaction selection
- **SimTests** – Provides unit-style validation of simulation behavior

---

## Simulation Flow

At each step:

- Valid reacting neighbors are identified
- Reaction propensities are calculated
- A reaction is selected probabilistically
- The tile state and concentrations are updated  