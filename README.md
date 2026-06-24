# Parallel Monte Carlo Integration and Option Pricing

**C++ | CUDA | OpenMP | CMake**

Monte Carlo framework for high-dimensional integration and option-pricing experiments, developed for the Advanced Methods for Scientific Computing course at Politecnico di Milano.

The project includes a generic integration engine, finance-oriented Monte Carlo pricing utilities, and separate CPU/OpenMP and CUDA entry points for comparing parallel execution strategies.

## Overview

This repository implements Monte Carlo estimators for numerical integration and financial option-pricing workloads. The codebase is split between a general-purpose integration module and an option-pricing module built around assets, option parameters, pricing utilities, and Monte Carlo simulation logic.

The goal is to expose the tradeoff that makes Monte Carlo methods attractive in high-dimensional settings: accuracy improves with more samples, while runtime depends heavily on parallel execution and random-sampling throughput.

## Numerical and Computing Approach

Core components:

- Monte Carlo integration over hyperspheres, hyperrectangles, and hypercubes
- Configurable sample count, dimension, domain parameters, and target function
- OpenMP parallelism for CPU execution
- CUDA implementation for GPU-oriented option-pricing experiments
- Historical asset CSV inputs for finance experiments
- CMake build structure with separate CUDA target

## Repository Structure

- `src/integration/`: generic Monte Carlo integration implementation
- `src/optionpricing/`: option-pricing domain logic and simulation utilities
- `include/`: headers for integration and finance components
- `CUDA/`: CUDA option-pricing implementation and build target
- `data/`: sample historical asset data used by pricing experiments
- `external/`: third-party parser dependency used for configurable functions

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

CUDA target:

```bash
cd CUDA
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Run

CPU/OpenMP executable:

```bash
./mainOmp
```

The program prompts for:

- integration domain
- number of random samples
- dimensionality
- domain parameters
- function to integrate

It returns the estimated integral and runtime.

## Notes

This is a university project built to study Monte Carlo methods and parallel execution. Public benchmark tables are not currently included in the repository; future polishing should add a small reproducible benchmark comparing serial CPU, OpenMP, and CUDA runs on a fixed option-pricing workload.

## Project Team

Developed by Emanuele Bellini, Luca Simei, and Luca Tramacere at Politecnico di Milano.
