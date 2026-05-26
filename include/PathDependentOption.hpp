#pragma once
#include <vector>

class PathDependentOption {
public:
    virtual ~PathDependentOption() = default;
    
    virtual double calculatePayoff(const std::vector<double>& path) const = 0;
};

class AsianCallOptionArithmetic : public PathDependentOption {
private:
    double strike;
public:
    AsianCallOptionArithmetic(double K);
    double calculatePayoff(const std::vector<double>& path) const override;
};