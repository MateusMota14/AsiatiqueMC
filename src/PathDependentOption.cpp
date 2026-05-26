#include "PathDependentOption.hpp"
#include <numeric>
#include <algorithm>

AsianCallOptionArithmetic::AsianCallOptionArithmetic(double K) : strike(K) {}

double AsianCallOptionArithmetic::calculatePayoff(const std::vector<double>& path) const {
    double sum = std::accumulate(path.begin(), path.end(), 0.0);
    double average = sum / path.size();
    
    return std::max(average - strike, 0.0);
}