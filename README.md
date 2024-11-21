# Computer Architecture Design Space Exploration Project

## Overview
Design space exploration (DSE) framework for computer architecture optimization using SimpleScalar. Explores 18-dimensional processor pipeline and memory hierarchy design space to find:
- Best overall performance (minimizing execution time)
- Maximum energy efficiency (minimizing energy-delay product)

## Project Structure

### Files
```
.
├── 431project.cpp/h         # Main framework and data structures
├── 431projectUtils.cpp      # Utility functions
├── YOURCODEHERE.cpp         # DSE heuristic implementation
├── runprojectsuite.sh       # SimpleScalar simulation runner
├── logs/                    # Exploration logs
└── rawProjectOutputData/    # Simulation outputs
```

### Key Components

#### Configuration Generator
```cpp
string generateNextConfigurationProposal(
    string currentconfiguration,
    string bestEXECconfiguration, 
    string bestEDPconfiguration,
    int optimizeforEXEC, 
    int optimizeforEDP
);
```
- Implements PSU ID-based DSE heuristic
- Dimension groups: FPU → Cache → Branch Predictor → Core
- Limited to 1000 design points
- Maintains exploration state

#### Configuration Validator
```cpp
int validateConfiguration(string configuration);
```

Cache hierarchy constraints:
- L1 block size ≥ 8B (instruction fetch queue size)
- L2 block size: 2×L1 to 128B
- L1 cache: 2KB - 64KB
- L2 cache: 32KB - 1MB
- L2 size ≥ 2×(L1I + L1D)

#### Cache Latency Calculator
```cpp
string generateCacheLatencyParams(string halfBakedConfig);
```

Latency rules:
- L1 cache (2KB-64KB):
  - Base: 1-6 cycles by size
  - +1 cycle: 2-way
  - +2 cycles: 4-way
  - +3 cycles: 8-way

- L2 cache (32KB-1MB):
  - Base: 5-10 cycles by size
  - +1 to +4 cycles for 2-way to 16-way

## Design Space

### Core Configuration
- Width: [1, 2, 4, 8]
- Scheduling: [in-order, out-of-order]

### Cache Configuration
- L1 data/instruction:
  - Block size: [8B, 16B, 32B, 64B]
  - Sets: [32..8192]
  - Associativity: [1, 2, 4]
- L2 unified:
  - Block size: [16B..128B]
  - Sets: [256..131072]
  - Associativity: [1..16]
- Replacement: [LRU, FIFO, random]

### Branch Predictor
- Schemes: [nottaken, bimod, 2lev, comb]
- RAS entries: [1, 2, 4, 8]
- BTB configuration: [128:16 to 2048:1]

### FPU
- Width: [1, 2, 4, 8]

## Usage

### Prerequisites
- SimpleScalar simulator
- CSE Linux Lab access
- VPN for remote execution

### Building
```bash
make clean
make
```

### Running
```bash
# Performance optimization
./DSE performance

# Energy efficiency optimization
./DSE energy
```

### Output Files
- `ExecutionTime.log`: Performance metrics
- `EnergyEfficiency.log`: Energy metrics
- `*.best`: Best configurations
- `rawProjectOutputData/`: Simulation data

## Implementation Notes

### Exploration Strategy
1. Group dimensions by functionality
2. Order by PSU ID calculation
3. Per dimension:
   - Test all values
   - Select optimal
   - Fix and continue

### Key Constraints
- 1000 design point maximum
- Baseline: `0 0 0 5 0 5 0 2 2 2 0 1 0 1 2 2 2 5`
- Memory latency: 51+7 cycles (8 words)

