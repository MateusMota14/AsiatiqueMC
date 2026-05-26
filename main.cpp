#include "MarketData.hpp"
#include "PathGenerator.hpp"
#include "PathDependentOption.hpp"
#include "MonteCarloEngine.hpp"
#include <iostream>
#include <chrono>

int main() {
    MarketData market;
    market.S0 = 100.0;     
    market.r = 0.05;       
    market.sigma = 0.20;   
    market.T = 1.0;        

    int steps = 252;                   
    long long simulations = 1000000;   
    double strike = 100.0;             

    std::cout << "Inicializando Simulador de Monte Carlo com Calculo de Gregas..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();

    PathGenerator pathGen(market, steps); 
    AsianCallOptionArithmetic asianCall(strike);
    MonteCarloEngine mcEngine(market, steps, simulations);

    SimulationResult baseResult = mcEngine.runSimulation(pathGen, asianCall);

    double h_S = 0.1; 
    
    MarketData market_S_plus = market;
    market_S_plus.S0 += h_S;
    PathGenerator pathGen_S_plus(market_S_plus, steps);
    MonteCarloEngine mcEngine_S_plus(market_S_plus, steps, simulations);
    SimulationResult result_S_plus = mcEngine_S_plus.runSimulation(pathGen_S_plus, asianCall);

    MarketData market_S_minus = market;
    market_S_minus.S0 -= h_S;
    PathGenerator pathGen_S_minus(market_S_minus, steps);
    MonteCarloEngine mcEngine_S_minus(market_S_minus, steps, simulations);
    SimulationResult result_S_minus = mcEngine_S_minus.runSimulation(pathGen_S_minus, asianCall);

    double delta = (result_S_plus.price - result_S_minus.price) / (2.0 * h_S);

    double h_v = 0.001; 

    MarketData market_v_plus = market;
    market_v_plus.sigma += h_v;
    PathGenerator pathGen_v_plus(market_v_plus, steps);
    MonteCarloEngine mcEngine_v_plus(market_v_plus, steps, simulations);
    SimulationResult result_v_plus = mcEngine_v_plus.runSimulation(pathGen_v_plus, asianCall);

    MarketData market_v_minus = market;
    market_v_minus.sigma -= h_v;
    PathGenerator pathGen_v_minus(market_v_minus, steps);
    MonteCarloEngine mcEngine_v_minus(market_v_minus, steps, simulations);
    SimulationResult result_v_minus = mcEngine_v_minus.runSimulation(pathGen_v_minus, asianCall);

    double vega = (result_v_plus.price - result_v_minus.price) / (2.0 * h_v);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "\n==========================================" << std::endl;
    std::cout << "Preco Estimado da Opcao: " << baseResult.price << std::endl;
    std::cout << "Erro Padrao Estatistico: " << baseResult.standardError << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Delta (Sensibilidade ao Preco): " << delta << std::endl;
    std::cout << "Vega  (Sensibilidade a Vol):    " << vega << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Tempo Total de Execucao: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}