# Reflection Report

## Activity Description
The expectation of this activity was to develop a DNA-based tile reaction network (TRN) simulator capable of visualizing molecular reactions over time. Each pixel in the simulation represents a distinct reactant, enabling high-resolution spatial tracking of the network. My tasks included implementing the simulation logic, generating visual outputs for reaction dynamics at each time step, and organizing the project for clarity. Additionally, I scaled the simulator to handle **generalized reactions**, making it applicable to arbitrary DNA networks instead of being limited to a single predefined system.

## Technical Decisions
Key technical decisions made during the project:
- **Simulation approach:** Used discrete-time steps to accurately capture reaction dynamics.
- **Visualization:** Each pixel represents a reactant, with color intensity or mapping representing molecular concentrations.
- **Generalization:** Designed the simulator to accept arbitrary reactions and reactants, increasing flexibility for multiple DNA networks.
- **Data handling:** Efficient storage of state matrices per time step allowed playback and analysis without excessive memory use.
- **Tools and language:** Implemented in C++ and Python for rapid development and easy visualization.
- **Modularity:** Structured code into simulation, visualization, analysis, and testing modules for maintainability.

## Contributions
I participated individually. My contributions included:
- Designing and implementing the core TRN simulation logic.
- Creating visual outputs of reactant states for each time step.
- Scaling the simulator to handle generalized reactions for flexibility across DNA networks.
- Writing a testing file to validate simulator functionality.
- Organizing project structure and documentation for clarity and reproducibility.

## Quality Assessment
I assess my participation as high-quality, as the simulator meets the primary objectives: visualizing molecular reactions over time with pixel-level representation and supporting generalized reaction sets. It also is well documented and very thorough. If I could redo this activity, I would:
- Optimize rendering for larger networks.
- Add interactive controls for zooming, filtering, or adjusting simulation parameters in real-time.
- Implement automated validation against experimental data to enhance accuracy.