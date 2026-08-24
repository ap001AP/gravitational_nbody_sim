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
    std::deque<Vector2> trail;
    size_t maxTrailLength = 1000;
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

    int stepsPerFrame = 20;

    // raylib window
    InitWindow(screenWidth, screenHeight, "N Body Gravitional Simulator");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        for (size_t x = 0; x < (stepsPerFrame + 1); ++x) {

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
            }

            // Euler Update
            for (size_t i = 0; i < n; ++i){
                // acceleration
                bodies[i].ax = bodies[i].Fx_total / bodies[i].m;
                bodies[i].ay = bodies[i].Fy_total / bodies[i].m;
                // velocity
                bodies[i].vx = bodies[i].vx + (tau * bodies[i].ax);
                bodies[i].vy = bodies[i].vy + (tau * bodies[i].ay);
                // position
                bodies[i].x = bodies[i].x + (tau * bodies[i].vx);
                bodies[i].y = bodies[i].y + (tau * bodies[i].vy);

                Vector2 currPos = Vector2{(float)(bodies[i].x), (float)(bodies[i].y)};
                bodies[i].trail.push_front(currPos);
                if (bodies[i].trail.size() > bodies[i].maxTrailLength)
                    bodies[i].trail.pop_back();

                // reset force totals
                bodies[i].Fx_total = 0;
                bodies[i].Fy_total = 0;
            }
            // kinetic energy
            // K_total += (1 / 2) * bodies[i].m * ((bodies[i].vx * bodies[i].vx) + (bodies[i].vy * bodies[i].vy));
        }
        BeginDrawing();
            ClearBackground(BLACK);

            for (size_t i = 0; i < n; ++i){
                // check that we have at least 2 points
                if (bodies[i].trail.size() > 1){
                    // trail points from newest -----> oldest of a body
                    for (size_t j = 0; j < (bodies[i].trail.size() - 1); ++j){

                        Vector2 currPixelPos = {
                            (float)(screenOriginX + (bodies[i].trail[j].x * scale)),
                            (float)(screenOriginY - (bodies[i].trail[j].y * scale))};
                        
                        Vector2 nextPixelPos = {
                            (float)(screenOriginX + (bodies[i].trail[j+1].x * scale)),
                            (float)(screenOriginY - (bodies[i].trail[j+1].y * scale))};

                        // fading the line
                        float opac = 1.0f - ((float)j / (float)bodies[i].trail.size());
                        Color line = Fade(RED, opac * 0.7f);

                        float baseThickness = 4.0f;
                        float currThickness = baseThickness * opac;
                        DrawLineEx(currPixelPos, nextPixelPos, currThickness, line);
                    }
                }
                float radius = log10(bodies[i].m);
                int screenX = (int)(screenOriginX + (bodies[i].x * scale));
                int screenY = (int)(screenOriginY - (bodies[i].y * scale));
                float screenR = (radius * radiusScale);

                DrawCircle(screenX, screenY, screenR, WHITE);
            }
        EndDrawing();
    }
    CloseWindow();

        /*
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
        */

       //double E_total = K_total + U_total;
       //std::cout << "Total Energy is : " << E_total << "\n";

    return 0;
}