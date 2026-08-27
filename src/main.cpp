#include <iostream>
#include <vector>
#include <raylib.h>
#include <cmath>
#include <iomanip>
#include <fstream>

// G = 6.67430e-11
constexpr double G = 1.000;

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

void updatePhysics(std::vector<Body>& bodies, double tau, double& K_total, double& U_total, double& E_total){

    size_t n = bodies.size();

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
    // calculating initial energy
    K_total = 0;
    for (size_t i = 0; i < n; ++i){
        K_total += (0.5) * bodies[i].m * ((bodies[i].vx * bodies[i].vx) + (bodies[i].vy * bodies[i].vy));
    }

    U_total = 0;
    for (size_t i = 0; i < (n-1); ++i)
    {
        for (size_t j = n - 1; j > i; --j)
        {

            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;

            double r = sqrt((dx * dx) + (dy * dy));

            U_total += (-1 * (G * bodies[i].m * bodies[j].m)) / r;
        }
    }

    E_total = K_total + U_total;
}

/*
void saveSeparationData(std::vector<double> timeData, std::vector<double> separationData){
    std::ofstream file("../analysis/separation.csv");

    file << "Time,Separation\n";

    for (size_t i = 0; i < timeData.size(); ++i) {
        file << timeData[i] << "," << separationData[i] << "\n";
    }

    file.close();
}
*/

void saveEnergyData(std::vector<double> timeData, std::vector<double> energyData){
        std::ofstream file("../analysis/energy.csv");

    file << "Time,Energy\n";
    file << std::setprecision(15);

    for (size_t i = 0; i < timeData.size(); ++i) {
        file << timeData[i] << "," << energyData[i] << "\n";
    }

    file.close();

}

int main()
{
    
    Body mass1{
        10,
        -50, 0,       // position
        0, 3,         // velocity
        0, 0,         // acceleration
        0, 0          // force
    };

    Body mass2{
        10,
        50, 0,
        0, -3,
        0, 0,
        0, 0
    };

    Body mass3{
        10,
        0, 50,
        3, 0,
        0, 0,
        0, 0
    };
    
/*
    Body mass1{
        1000,
        0, 0, // position
        0, 0, // velocity
        0, 0, // acceleration
        0, 0}; // force

    Body mass2{
        10,
        100, 0,
        0, 3.16,
        0, 0,
        0, 0};

    Body mass3{
        5,
        100, 200,
        -3, 0,
        0, 0,
        0, 0};
*/
    
    std::vector<Body> bodiesA;
    bodiesA.push_back(mass1);
    bodiesA.push_back(mass2);
    bodiesA.push_back(mass3);
    
    /*
    std::vector<Body> bodiesB;
    bodiesB.push_back(mass1);
    bodiesB.push_back(mass2);
    bodiesB.push_back(mass3);

    bodiesB[2].x += 0.1;
    */

    size_t nA = bodiesA.size();
    // size_t nB = bodiesB.size();

    
    // calculating initial energy
    double K_total = 0;
    for (size_t i = 0; i < nA; ++i){
        K_total += (0.5) * bodiesA[i].m * ((bodiesA[i].vx * bodiesA[i].vx) + (bodiesA[i].vy * bodiesA[i].vy));
    }

    double U_total = 0;
    for (size_t i = 0; i < (nA-1); ++i)
    {
        for (size_t j = nA - 1; j > i; --j)
        {

            double dx = bodiesA[j].x - bodiesA[i].x;
            double dy = bodiesA[j].y - bodiesA[i].y;

            double r = sqrt((dx * dx) + (dy * dy));

            U_total += (-1 * (G * bodiesA[i].m * bodiesA[j].m)) / r;
        }
    }

    double E_total = K_total + U_total;

    // timestep 
    double tau = 0.01;
    double simulationTime = 0.0;
    double simulationEnd = 1000.0;

    double nextPrintTime = 10.0;

    std::vector<double> timeData;
    //std::vector<double> separationData;
    std::vector<double> energyData;

    timeData.push_back(0.0);
    energyData.push_back(E_total);

    //raylib initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    double screenOriginX = screenWidth / 2.0;
    double screenOriginY = screenHeight / 2.0;

    double scale = 1.0; // pixels per unit
    float radiusScale = 2.0;

    int stepsPerFrame = 10;


    double initialEnergy = E_total;
    double finalEnergy = E_total;

    // bool validationComplete = false;

    // raylib window
    InitWindow(screenWidth, screenHeight, "N Body Gravitional Simulator");

    SetTargetFPS(60);

    while (!WindowShouldClose() && simulationTime < simulationEnd) {

        for (size_t x = 0; x < stepsPerFrame; ++x) {

            updatePhysics(bodiesA, tau, K_total, U_total, E_total);
            //updatePhysics(bodiesB, tau);

            simulationTime += tau;
        }

        timeData.push_back(simulationTime);
        energyData.push_back(E_total);

        /*
        double dx = bodiesA[2].x - bodiesB[2].x;
        double dy = bodiesA[2].y - bodiesB[2].y;

        double separation = sqrt(dx * dx + dy * dy);

        timeData.push_back(simulationTime);
        separationData.push_back(separation);

        if (simulationTime >= nextPrintTime) {
            std::cout << "Time: " << simulationTime << " Separation: " << separation << "\n";
            nextPrintTime += 10.0;
        }
        */

        /*
        // kinetic energy
        K_total = 0;
        for (size_t i = 0; i < n; ++i){
            K_total += (0.5) * bodies[i].m * ((bodies[i].vx * bodies[i].vx) + (bodies[i].vy * bodies[i].vy));
        }

        // potential energy 
        U_total = 0;
        for (size_t i = 0; i < (n-1); ++i)
        {
            for (size_t j = n - 1; j > i; --j)
            {

                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;

                double r = sqrt((dx * dx) + (dy * dy));

                U_total += (-1 * (G * bodies[i].m * bodies[j].m)) / r;
            }
        }

        E_total = K_total + U_total;

        if (simulationTime >= sim_units && !validationComplete)
        {
            double energyError = ((std::abs(finalEnergy - initialEnergy)) / std::abs(initialEnergy)) * 100.0;
            std::cout << std::setprecision(10);
            std::cout << "Initial Energy: " << initialEnergy << " Final Energy: " << finalEnergy << "\n";
            std::cout << "Error: " << energyError << "%\n";
            validationComplete = true;
        }
        */

        BeginDrawing();
            ClearBackground(BLACK);

            for (size_t i = 0; i < nA; ++i){
                // check that we have at least 2 points
                if (bodiesA[i].trail.size() > 1){
                    // trail points from newest -----> oldest of a body
                    for (size_t j = 0; j < (bodiesA[i].trail.size() - 1); ++j){

                        Vector2 currPixelPos = {
                            (float)(screenOriginX + (bodiesA[i].trail[j].x * scale)),
                            (float)(screenOriginY - (bodiesA[i].trail[j].y * scale))};
                        
                        Vector2 nextPixelPos = {
                            (float)(screenOriginX + (bodiesA[i].trail[j+1].x * scale)),
                            (float)(screenOriginY - (bodiesA[i].trail[j+1].y * scale))};

                        // fading the line
                        float opac = 1.0f - ((float)j / (float)bodiesA[i].trail.size());
                        Color line = Fade(RED, opac * 0.7f);

                        float baseThickness = 4.0f;
                        float currThickness = baseThickness * opac;
                        DrawLineEx(currPixelPos, nextPixelPos, currThickness, line);
                    }
                }
                //Draw body
                float radius = log10(bodiesA[i].m);
                int screenX = (int)(screenOriginX + (bodiesA[i].x * scale));
                int screenY = (int)(screenOriginY - (bodiesA[i].y * scale));
                float screenR = (radius * radiusScale);

                DrawCircle(screenX, screenY, screenR, WHITE);
            }

            /*
            for (size_t i = 0; i < nB; ++i){
                // check that we have at least 2 points
                if (bodiesB[i].trail.size() > 1){
                    // trail points from newest -----> oldest of a body
                    for (size_t j = 0; j < (bodiesB[i].trail.size() - 1); ++j){

                        Vector2 currPixelPos = {
                            (float)(screenOriginX + 200 + (bodiesB[i].trail[j].x * scale)),
                            (float)(screenOriginY - (bodiesB[i].trail[j].y * scale))};
                        
                        Vector2 nextPixelPos = {
                            (float)(screenOriginX + 200 + (bodiesB[i].trail[j+1].x * scale)),
                            (float)(screenOriginY - (bodiesB[i].trail[j+1].y * scale))};

                        // fading the line
                        float opac = 1.0f - ((float)j / (float)bodiesB[i].trail.size());
                        Color line = Fade(RED, opac * 0.7f);

                        float baseThickness = 4.0f;
                        float currThickness = baseThickness * opac;
                        DrawLineEx(currPixelPos, nextPixelPos, currThickness, line);
                    }
                }
                //Draw body
                float radius = log10(bodiesB[i].m);
                int screenX = (int)(screenOriginX + 200 + (bodiesB[i].x * scale));
                int screenY = (int)(screenOriginY - (bodiesB[i].y * scale));
                float screenR = (radius * radiusScale);

                DrawCircle(screenX, screenY, screenR, WHITE);
            }
            */

        EndDrawing();
    }
    CloseWindow();
    finalEnergy = E_total;
    double energyError = ((std::abs(finalEnergy - initialEnergy)) / std::abs(initialEnergy)) * 100.0;
    std::cout << "Error: " << energyError << "%\n";

    //saveSeparationData(timeData, separationData);
    saveEnergyData(timeData, energyData);

    return 0;
}