#pragma once
#include "PathGenerator.hpp"
#include "PathDependentOption.hpp"
#include "MarketData.hpp"

struct SimulationResult {
    double price;
    double standardError;
};

class MonteCarloEngine {
private:
    MarketData marketData;
    int numSteps;
    long long numSimulations;

public:
    MonteCarloEngine(const MarketData& data, int steps, long long sims);
    
    SimulationResult runSimulation(PathGenerator& pathGen, const PathDependentOption& option);
};