# N-Body Gravitational Simulator

A C++ N-body gravity simulator built from scratch, progressing from a basic Euler-integrated brute-force simulation toward more advanced integrators, spatial partitioning, and eventually research-adjacent performance and accuracy. Built as a hands-on project while learning C++, with all code written manually.

## Project Structure

This project is organized by branch, with each branch representing a stage of development:

- **[`v1_euler_update`](https://github.com/ap001AP/gravitational_nbody_sim/tree/v1_euler_update)** — the core Phase 1 implementation: O(N²) pairwise gravity, Euler integration, real-time SFML/raylib rendering with trail visualization.
- **[`v1_testing`](https://github.com/ap001AP/gravitational_nbody_sim/tree/v1_testing)** — validation of the Phase 1 implementation against defined milestones: 2-body circular orbit stability, 3-body chaotic divergence, sensitivity to initial conditions, and energy conservation analysis across multiple orbital configurations.

## Roadmap

The project is planned across three phases of increasing complexity:

- **Phase 1**: direct O(N²) force calculation, Euler integration, 2D simulation, basic real-time visualization. *(Completed)*
- **Phase 2**: Velocity Verlet integration, Barnes-Hut octree (3D), adaptive timestep, camera controls, octree visualization. *(Active)*
- **Phase 3**: multithreaded/GPU-accelerated force calculation, higher-order symplectic integrators, large-N galaxy-scale simulation, instanced rendering, playback controls.

## Key Findings So Far (Phase 1)

- Euler integration conserves energy reasonably well over short-to-moderate timescales, but conservation quality depends heavily on the orbital configuration — symmetric, equal-mass setups show far less drift (~10⁻⁷ %) than eccentric, high-mass-ratio setups with close encounters (~10⁻³ %).
- The three-body system shows clear sensitivity to initial conditions, with a small perturbation (Δx = 0.1) leading to sustained, growing divergence over time — though sub-exponential rather than strictly chaotic in the Lyapunov sense over the tested timescale.

See the `v1_testing` branch README for full validation details, plots, and GIFs.

## Dependencies
- [raylib](https://www.raylib.com/)
- A C++ compiler supporting C++17 or later
- Python 3 with `pandas` and `matplotlib` (for analysis/plotting, not required to run the simulator itself)

## Status
Actively in development. Phase 1 complete and validated; Phase 2 (Velocity Verlet + Barnes-Hut, 3D) in progress.