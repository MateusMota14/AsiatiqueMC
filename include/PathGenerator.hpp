// include/PathGenerator.hpp
#pragma once
#include "MarketData.hpp"
#include <vector>
#include <random>

class PathGenerator {
private:
    MarketData data;
    int numSteps;
    double dt;
    double drift;
    double vol;
    
    std::mt19937 generator;
    std::normal_distribution<double> normalDist;

public:
    PathGenerator(const MarketData& marketData, int steps, unsigned int seed = 0);
    
    void generateAntitheticPaths(std::vector<double>& path1, std::vector<double>& path2);
};