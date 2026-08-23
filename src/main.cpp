#include <iostream>
#include <vector>
#include <raylib.h>

constexpr double G = 6.67430e-11;

// Declaring a Body structure
struct Body {
    double m;
    double x, y;
    double vx, vy;
    double ax, ay;
    double Fx_total, Fy_total;
};

int main()
{

    Body mass1{
        1000,
        0, 0,
        0, 0,
        0, 0,
        0, 0};

    Body mass2{
        10,
        100, 0,
        0, 5,
        0, 0,
        0, 0};

    Body mass3{
        5,
        100, 200,
        -3, 0,
        0, 0,
        0, 0};

    // vector of structs
    std::vector<Body> bodies;

    bodies.push_back(mass1);
    bodies.push_back(mass2);
    bodies.push_back(mass3);

    size_t n = bodies.size();

    double K_total = 0;

    // timestep 
    double tau = 0.01;

    // simulation 
    int sim_time = 100;

    int N = static_cast<int>(sim_time / tau);

    //raylib initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    double screenOriginX = screenWidth / 2.0;
    double screenOriginY = screenHeight / 2.0;

    double scale = 1.0; // pixels per unit
    float radiusScale = 2.0;

    // raylib window
    InitWindow(screenWidth, screenHeight, "N Body Gravitional Simulator");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            for (size_t i = 0; i < n; ++i){

                float radius = log10(bodies[i].m);
                int screenX = (int)(screenOriginX + (bodies[i].x * scale));
                int screenY = (int)(screenOriginY - (bodies[i].y * scale));
                float screenR = (radius * radiusScale);

                DrawCircle(screenX, screenY, screenR, WHITE);
            }
        EndDrawing();
    }
    CloseWindow();

    for (int step = 0; step < N; ++step) {

        // Force Calculation
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {

                if (i == j)
                    continue;

                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;

                double r = sqrt((dx * dx) + (dy * dy));
                double F = (G * bodies[i].m * bodies[j].m) / (r * r);

                double Fx = F * (dx / r);
                double Fy = F * (dy / r);

                bodies[i].Fx_total += Fx;
                bodies[i].Fy_total += Fy;
            }

            // Euler Update
            // acceleration
            bodies[i].ax = bodies[i].Fx_total / bodies[i].m;
            bodies[i].ay = bodies[i].Fy_total / bodies[i].m;

            // velocity
            bodies[i].vx = bodies[i].vx + (tau * bodies[i].ax);
            bodies[i].vy = bodies[i].vy + (tau * bodies[i].ay);

            // position
            bodies[i].x = bodies[i].x + (tau * bodies[i].vx);
            bodies[i].y = bodies[i].y + (tau * bodies[i].vy);

            // kinetic energy
            K_total += (1 / 2) * bodies[i].m * ((bodies[i].vx * bodies[i].vx) + (bodies[i].vy * bodies[i].vy));
        }

        // potential energy 
        int j = n - 1;
        double U_total = 0;

        for (size_t i = 0; i < j; ++i){
            while (i < j){

                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;

                double r = sqrt((dx * dx) + (dy * dy));

                U_total += (-1 * (G * bodies[i].m * bodies[j].m)) / r;

                j -= 1;
            }
            j = n - 1;
        }

        double E_total = K_total + U_total;
        std::cout << "Total Energy is : " << E_total << "\n";

    }

    return 0;
}