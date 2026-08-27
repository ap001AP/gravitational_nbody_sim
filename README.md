# N-Body Gravity Simulator — Euler Integration 

## Overview
A basic N-body gravitational simulator written in C++, using direct O(N²) pairwise force summation and Euler integration. Bodies interact in 2D under Newtonian gravity, with real-time rendering via raylib. This branch establishes a working baseline before moving to more advanced integrators and spatial partitioning in later levels.

## Physics Implemented
- **Newtonian gravity**: pairwise force calculation `F = Gm1m2/r²` between every pair of bodies (O(N²), nested loop)
- **Euler integration**: acceleration computed from total force, velocity updated from acceleration, position updated from velocity
- **Units**: real SI units, using the true gravitational constant `G = 6.67430e-11`
- **Trail rendering**: each body stores a bounded history of recent positions, rendered as a fading trail to visualize orbital paths

## Known Limitations
- Euler integration is a first-order method and does not conserve energy exactly — total energy will drift over time. This is expected behavior for this integrator, not a bug.
- Energy tracking (kinetic + potential) is present in the code but currently commented out / unused in this branch — it was scaffolded for validation purposes but not active during normal runs. See the [`v1_testing`](https://github.com/ap001AP/gravitational_nbody_sim/tree/v1_testing) branch for active energy conservation measurement and analysis.
- Using real SI values for `G` alongside small, simulation-friendly mass/distance/velocity values means the resulting gravitational forces are extremely weak relative to real astronomical scales — this branch is focused on validating the simulation mechanics, not modeling a physically realistic system.
- No spatial partitioning (e.g. Barnes-Hut) — force calculation is O(N²), which will not scale to large N. This is addressed in Level 2.
- No data export (CSV) in this branch — output is visual only, via the raylib render loop. Data logging for validation was added separately in [`v1_testing`](https://github.com/ap001AP/gravitational_nbody_sim/tree/v1_testing).

## Code Structure
- `Body` struct — holds mass, position (x, y), velocity (vx, vy), acceleration (ax, ay), accumulated force (Fx_total, Fy_total), and a bounded trail history for rendering
- `main()` — sets up the initial three-body configuration, runs the simulation loop (force calculation + Euler update, repeated `stepsPerFrame` times per rendered frame), and handles raylib rendering. Physics and rendering are not yet separated into distinct functions in this branch.

## Dependencies
- [raylib](https://www.raylib.com/)
- A C++ compiler supporting C++17 or later (developed using g++ on macOS)

## Build & Run
\`\`\`
g++ main.cpp -o main -I/usr/local/include -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./main
\`\`\`
(Framework flags shown are for macOS; adjust linking as needed on other platforms.)

## What's Next
See the [`v1_testing`](https://github.com/ap001AP/gravitational_nbody_sim/tree/v1_testing) branch for validation results (2-body/3-body behavior, sensitivity to initial conditions, and energy conservation analysis — using refactored code with active energy tracking and CSV export). Phase 2 will introduce Velocity Verlet integration and a Barnes-Hut octree for improved accuracy and performance.
