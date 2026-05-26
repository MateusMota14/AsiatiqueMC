# Monte Carlo Simulator for Asian Options Pricing and Risk Management

This project is a high-performance derivative pricing engine developed in **C++17** designed to price complex path-dependent financial contracts, with a specific focus on **Arithmetic Average Asian Options**.

Because Arithmetic Asian options do not possess an exact analytical closed-form solution (as the sum of log-normal variables is not itself log-normal), this project implements a **Monte Carlo Simulation** based on **Geometric Brownian Motion (GBM)**. It integrates production-grade optimizations, including multi-threaded computing and variance reduction techniques.

---

## 🧠 Theoretical Framework & Stochastic Modeling

### 1. Geometric Brownian Motion (GBM)
The asset price dynamics are modeled under the risk-neutral measure using the following Stochastic Differential Equation (SDE):

$$dS_t = r S_t dt + \sigma S_t dW_t$$

Where $r$ is the risk-free rate (acting as the drift), $\sigma$ is the instantaneous volatility, and $dW_t$ represents the increment of a Wiener process. Applying **Itô's Lemma**, the exact integration for the discrete temporal asset path is given by:

$$S_{t+\Delta t} = S_t \exp\left( \left(r - \frac{\sigma^2}{2}\right)\Delta t + \sigma \sqrt{\Delta t} \epsilon \right), \quad \epsilon \sim \mathcal{N}(0,1)$$

### 2. Asian Option Payoff
Unlike standard European options, the Asian option evaluated here is *path-dependent*, discretely monitoring the asset trajectory across $m$ steps. The payoff of an Arithmetic Average Asian Call option is defined as:

$$\text{Payoff} = \max\left(\frac{1}{m+1}\sum_{i=0}^{m} S_{t_i} - K, 0\right)$$

The current fair price of the derivative is the mathematical expectation of this discounted payoff under the risk-neutral measure, adjusted by the stochastic discount factor $e^{-rT}$.

---

## ⚡ Advanced Quantitative Engineering Techniques

### Variance Reduction: Antithetic Variates
To accelerate statistical convergence without doubling the computational cost of generating pseudo-random numbers, the engine utilizes mirror trajectories. For every random shock $\epsilon$ sampled, two path simulations are executed simultaneously:
* **Original Path:** Computed using the positive shock $+\epsilon$.
* **Antithetic Path:** Computed using the negative shock $-\epsilon$.

Since the Standard Normal distribution is perfectly symmetrical around zero, both paths represent perfectly valid and statistically identical stochastic trajectories. Being negatively correlated, the negative covariance between their respective payoffs dramatically shrinks the estimator's variance, effectively cutting the Standard Error in half.

### High-Performance Computing & Memory Management
* **Native Parallelization (OpenMP):** The main execution loop is distributed across multiple CPU cores via compiler directives. To prevent race conditions, a statistical reduction pattern is applied to the thread-shared accumulator variables for option price and variance.
* **Thread-Safe Random Number Generation:** The pseudo-random generator `std::mt19937` is not thread-safe. The engine mitigates memory corruption by instantiating an independent, localized generator per thread, dynamically seeded using hardware entropy (`std::random_device`) and high-resolution timestamps (`std::chrono`).
* **Zero-Allocation Loop:** Memory allocation for vectors holding the price trajectories (`std::vector<double>`) is performed exactly once per thread outside the parallel scope. Inside the loop, memory on the heap is overwritten rather than reallocated, eliminating a massive computational bottleneck.

---

## 📊 Risk Management: Numerical Calculation of Greeks

For hedging and risk management purposes, the simulator acts as a *Risk Engine*, computing the partial derivatives of the option price with respect to market parameters using the **Central Finite Difference** method.

### Delta ($\Delta$)
Measures the sensitivity of the option price to small changes in the underlying asset price ($S_0$). It is calculated by locally perturbing the initial spot price with a small shock $h_S$:

$$\Delta = \frac{\text{Price}(S_0 + h_S) - \text{Price}(S_0 - h_S)}{2h_S}$$

### Vega ($\nu$)
Measures the sensitivity of the option price to changes in the asset's volatility ($\sigma$). It is evaluated by applying an infinitesimal shock $h_\sigma$ to the volatility parameter:

$$\nu = \frac{\text{Price}(\sigma + h_\sigma) - \text{Price}(\sigma - h_\sigma)}{2h_\sigma}$$

The engine automatically coordinates the base simulation and the four perturbed simulations required to compute these derivatives with maximum statistical accuracy.

---

## 🛠️ Compilation and Execution

Ensure your compiler (`g++`) supports C++17 and has native support for OpenMP.

### 1. Compilation Command
Run the following command in your project root directory to compile all modules with maximum machine-code optimization (`-O3` forces advanced compiler techniques such as SIMD vectorization, loop unrolling, and function inlining):

```bash
g++ -O3 -std=c++17 -fopenmp main.cpp src/*.cpp -I include -o mc_simulator
```
### 2. Execution Command
Run the compiled native binary directly from your VS Code terminal:
On Windows
```bash
.\mc_simulator.exe
```
On Linux
```bash
./mc_simulator
```

