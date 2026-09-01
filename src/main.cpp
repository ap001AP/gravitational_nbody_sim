#include <iostream>
#include <vector>
#include <raylib.h>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <deque>

// G = 6.67430e-11
constexpr double G = 1.000;

// Declaring a Body structure
struct Body {
    double m;
    double x, y, z;
    double vx, vy, vz;
    double ax, ay, az;
    double Fx_total, Fy_total, Fz_total;
    std::deque<Vector3> trail;
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
            double dz = bodies[j].z - bodies[i].z;

            double r = sqrt((dx * dx) + (dy * dy) + (dz * dz));
            double F = (G * bodies[i].m * bodies[j].m) / (r * r);

            double Fx = F * (dx / r);
            double Fy = F * (dy / r);
            double Fz = F * (dz / r);

            bodies[i].Fx_total += Fx;
            bodies[i].Fy_total += Fy;
            bodies[i].Fz_total += Fz;
        }
    }

    // Velocity Verlet
    for (size_t i = 0; i < n; ++i){

        // current acceleration
        bodies[i].ax = bodies[i].Fx_total / bodies[i].m;
        bodies[i].ay = bodies[i].Fy_total / bodies[i].m;
        bodies[i].az = bodies[i].Fz_total / bodies[i].m;

        // new position
        bodies[i].x = bodies[i].x + (tau * bodies[i].vx) +(0.5*bodies[i].ax*(tau*tau));
        bodies[i].y = bodies[i].y + (tau * bodies[i].vy) +(0.5*bodies[i].ay*(tau*tau));
        bodies[i].z = bodies[i].z + (tau * bodies[i].vz) +(0.5*bodies[i].az*(tau*tau));

        // reset force totals
        bodies[i].Fx_total = 0;
        bodies[i].Fy_total = 0;
        bodies[i].Fz_total = 0;
    }
        
    // new acceleration
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {

            if (i == j)
                continue;

            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double dz = bodies[j].z - bodies[i].z;

            double r = sqrt((dx * dx) + (dy * dy) + (dz * dz));
            double F = (G * bodies[i].m * bodies[j].m) / (r * r);

            double Fx = F * (dx / r);
            double Fy = F * (dy / r);
            double Fz = F * (dz / r);

            bodies[i].Fx_total += Fx;
            bodies[i].Fy_total += Fy;
            bodies[i].Fz_total += Fz;
        }

    }

    for (size_t i = 0; i < n; ++i)
    {
        // copy current acceleration before we update it with new position
        double copy_ax = bodies[i].ax;
        double copy_ay = bodies[i].ay;
        double copy_az = bodies[i].az;

        // new acceleration
        bodies[i].ax = bodies[i].Fx_total / bodies[i].m;
        bodies[i].ay = bodies[i].Fy_total / bodies[i].m;
        bodies[i].az = bodies[i].Fz_total / bodies[i].m;

        // new velocity
        bodies[i].vx = bodies[i].vx + (0.5*(copy_ax + bodies[i].ax))*tau;
        bodies[i].vy = bodies[i].vy + (0.5*(copy_ay + bodies[i].ay))*tau;
        bodies[i].vz = bodies[i].vz + (0.5*(copy_az + bodies[i].az))*tau;

        Vector3 currPos = Vector3{(float)(bodies[i].x), (float)(bodies[i].y), (float)(bodies[i].z)};
        bodies[i].trail.push_front(currPos);
        if (bodies[i].trail.size() > bodies[i].maxTrailLength)
            bodies[i].trail.pop_back();

        // reset force totals
        bodies[i].Fx_total = 0;
        bodies[i].Fy_total = 0;
        bodies[i].Fz_total = 0;
    }

    // calculating energy (kinetic + potential)
    K_total = 0;
    for (size_t i = 0; i < n; ++i){
        K_total += (0.5) * bodies[i].m * ((bodies[i].vx * bodies[i].vx) + (bodies[i].vy * bodies[i].vy) + (bodies[i].vz * bodies[i].vz));
    }

    U_total = 0;
    for (size_t i = 0; i < (n-1); ++i)
    {
        for (size_t j = n - 1; j > i; --j)
        {
            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double dz = bodies[j].z - bodies[i].z;

            double r = sqrt((dx * dx) + (dy * dy) + (dz * dz));

            U_total += (-1 * (G * bodies[i].m * bodies[j].m)) / r;
        }
    }
    E_total = K_total + U_total;
}

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
        -50, 0, 50,       // position
        0, 3, 0,         // velocity
        0, 0, 0,        // acceleration
        0, 0, 0,          // force
    };

    Body mass2{
        10,
        50, 0, -50,
        0, -3, 0,
        0, 0, 0,
        0, 0, 0,
    };

    Body mass3{
        10,
        0, 50, 0,
        3, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };

    
/*
    Body mass1{
        1000,
        0, 0, 0, // position
        0, 0, 0,// velocity
        0, 0, 0, // acceleration
        0, 0, 0}; // force

    Body mass2{
        10,
        100, 0, 0,
        0, 3.16228, 0,
        0, 0, 0,
        0, 0, 0};

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
    

    size_t nA = bodiesA.size();
    
    // calculating initial energy (kinetic + potential)
    double K_total = 0;
    for (size_t i = 0; i < nA; ++i){
        K_total += (0.5) * bodiesA[i].m * ((bodiesA[i].vx * bodiesA[i].vx) + (bodiesA[i].vy * bodiesA[i].vy) + (bodiesA[i].vz * bodiesA[i].vz));
    }

    double U_total = 0;
    for (size_t i = 0; i < (nA-1); ++i)
    {
        for (size_t j = nA - 1; j > i; --j)
        {
            double dx = bodiesA[j].x - bodiesA[i].x;
            double dy = bodiesA[j].y - bodiesA[i].y;
            double dz = bodiesA[j].z - bodiesA[i].z;
            double r = sqrt((dx * dx) + (dy * dy) + (dz * dz));

            U_total += (-1 * (G * bodiesA[i].m * bodiesA[j].m)) / r;
        }
    }
    double E_total = K_total + U_total;

    // timestep 
    double tau = 0.01;
    double simulationTime = 0.0;
    double simulationEnd = 1000.0;

    std::vector<double> timeData;
    std::vector<double> energyData;
    timeData.push_back(0.0);
    energyData.push_back(E_total);

    //raylib initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    double screenOriginX = screenWidth / 2.0;
    double screenOriginY = screenHeight / 2.0;

    float radiusScale = 2.0;

    int stepsPerFrame = 10;

    double initialEnergy = E_total;
    double finalEnergy = E_total;

    // raylib window
    InitWindow(screenWidth, screenHeight, "N Body Gravitional Simulator");

    SetTargetFPS(60);

    // 3_D camera initialization
    Camera3D camera = {0};
    // elevated perspective to see the depth
    camera.position = (Vector3){0.0f, 120.0f, 150.0f};
    // looking at the center of coordinate space
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; 
    // setting y-axis as up
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose() && simulationTime < simulationEnd) {

        for (size_t x = 0; x < stepsPerFrame; ++x) {

            updatePhysics(bodiesA, tau, K_total, U_total, E_total);

            simulationTime += tau;
        }

        timeData.push_back(simulationTime);
        energyData.push_back(E_total);

        UpdateCamera(&camera, CAMERA_ORBITAL);
        BeginDrawing();
            ClearBackground(BLACK);

            // activate 3d rendering
            BeginMode3D(camera);

                for (size_t i = 0; i < nA; ++i){
                    // check that we have at least 2 points
                    if (bodiesA[i].trail.size() > 1){
                        // trail points from newest -----> oldest of a body
                        for (size_t j = 0; j < (bodiesA[i].trail.size() - 1); ++j){

                            Vector3 currWorldPos = bodiesA[i].trail[j];
                            Vector3 nextWorldPos = bodiesA[i].trail[j + 1];

                            // fading the line
                            float opac = 1.0f - ((float)j / (float)bodiesA[i].trail.size());
                            Color line = Fade(RED, opac * 0.7f);

                            DrawLine3D(currWorldPos, nextWorldPos, line);
                        }
                    }
                    //Draw body
                    float radius = log10(bodiesA[i].m) * radiusScale;
                    Vector3 bodypos = {(float)bodiesA[i].x, (float)bodiesA[i].y, (float)bodiesA[i].z};

                    DrawSphere(bodypos, radius, WHITE);
                    // overlay wire mesh for depth perception
                    DrawSphereWires(bodypos, radius, 8, 8, RED);
                }
            EndMode3D();
        EndDrawing();
    }
    CloseWindow();

    finalEnergy = E_total;
    double energyError = ((std::abs(finalEnergy - initialEnergy)) / std::abs(initialEnergy)) * 100.0;
    std::cout << "Error: " << energyError << "%\n";

    saveEnergyData(timeData, energyData);

    return 0;
}