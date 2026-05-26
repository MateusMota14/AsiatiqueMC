// src/MonteCarloEngine.cpp
#include "MonteCarloEngine.hpp"
#include <cmath>
#include <vector>
#include <omp.h>
#include <chrono>
#include <random>

MonteCarloEngine::MonteCarloEngine(const MarketData& data, int steps, long long sims)
    : marketData(data), numSteps(steps), numSimulations(sims) {}

SimulationResult MonteCarloEngine::runSimulation(PathGenerator& /*pathGen*/, const PathDependentOption& option) {
    double totalPayoff = 0.0;
    double totalSqPayoff = 0.0;
    
    std::random_device rd;
    unsigned int masterSeed = rd();

    long long halfSimulations = numSimulations / 2;

    #pragma omp parallel reduction(+:totalPayoff, totalSqPayoff)
    {
        int threadId = omp_get_thread_num();
        unsigned int threadSeed = masterSeed + threadId * 1000; 

        PathGenerator localPathGen(marketData, numSteps, threadSeed);
        std::vector<double> path1(numSteps + 1);
        std::vector<double> path2(numSteps + 1);

        #pragma omp for
        for (long long i = 0; i < halfSimulations; ++i) {
            localPathGen.generateAntitheticPaths(path1, path2);
            
            double payoff1 = option.calculatePayoff(path1);
            double payoff2 = option.calculatePayoff(path2);
            
            double pairwisePrice = 0.5 * (payoff1 + payoff2);
            
            totalPayoff += pairwisePrice;
            totalSqPayoff += (pairwisePrice * pairwisePrice);
        }
    } 
    
    double meanPayoff = totalPayoff / halfSimulations;
    double discountFactor = std::exp(-marketData.r * marketData.T);
    double optionPrice = meanPayoff * discountFactor;
    
    double variance = (totalSqPayoff / halfSimulations) - (meanPayoff * meanPayoff);
    double standardDeviation = std::sqrt(variance);
    double standardError = (standardDeviation / std::sqrt(halfSimulations)) * discountFactor;
    
    return {optionPrice, standardError};
}