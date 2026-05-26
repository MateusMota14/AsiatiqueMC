// src/PathGenerator.cpp
#include "PathGenerator.hpp"
#include <cmath>
#include <chrono>

PathGenerator::PathGenerator(const MarketData& marketData, int steps, unsigned int seed)
    : data(marketData), numSteps(steps), normalDist(0.0, 1.0) {
    
    dt = data.T / numSteps;
    drift = (data.r - 0.5 * data.sigma * data.sigma) * dt;
    vol = data.sigma * std::sqrt(dt);

    if (seed == 0) {
        std::random_device rd;
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        unsigned int dynamicSeed = static_cast<unsigned int>(now) ^ rd();
        generator.seed(dynamicSeed);
    } else {
        generator.seed(seed);
    }
}

void PathGenerator::generateAntitheticPaths(std::vector<double>& path1, std::vector<double>& path2) {
    path1[0] = data.S0;
    path2[0] = data.S0;
    
    for (int i = 1; i <= numSteps; ++i) {
        double epsilon = normalDist(generator);
        
        path1[i] = path1[i - 1] * std::exp(drift + vol * epsilon);
        path2[i] = path2[i - 1] * std::exp(drift + vol * (-epsilon));
    }
}