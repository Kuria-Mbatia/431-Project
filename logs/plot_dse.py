import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def read_best_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    # Split lines by comma and convert to list
    edp_line = lines[0].strip().split(',')
    time_line = lines[1].strip().split(',')
    return edp_line, time_line

def create_performance_plot(time_line):
    # Extract per-benchmark normalized execution times (every other value starting from index 5)
    bench_times = [float(time_line[i]) for i in range(6, 15, 2)]
    
    benchmarks = ['bzip2', 'mcf', 'hmmer', 'sjeng', 'equake', 'Geomean']
    normalized_times = bench_times + [float(time_line[2])]  # Add geomean
    
    plt.figure(figsize=(10, 6))
    bars = plt.bar(benchmarks, normalized_times)
    plt.axhline(y=1, color='r', linestyle='--', label='Baseline')
    
    plt.title('Normalized Execution Time per Benchmark (Best Performance Configuration)')
    plt.ylabel('Normalized Execution Time')
    plt.xticks(rotation=45)
    plt.grid(True, axis='y', linestyle='--', alpha=0.7)
    
    # Add value labels on top of bars
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.2f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('normalized_execution_time.png')

def create_edp_plot(edp_line):
    # Extract per-benchmark normalized EDP values (every other value starting from index 5)
    bench_edp = [float(edp_line[i]) for i in range(6, 15, 2)]
    
    benchmarks = ['bzip2', 'mcf', 'hmmer', 'sjeng', 'equake', 'Geomean']
    normalized_edp = bench_edp + [float(edp_line[1])]  # Add geomean
    
    plt.figure(figsize=(10, 6))
    bars = plt.bar(benchmarks, normalized_edp)
    plt.axhline(y=1, color='r', linestyle='--', label='Baseline')
    
    plt.title('Normalized Energy-Delay Product per Benchmark (Best EDP Configuration)')
    plt.ylabel('Normalized EDP')
    plt.xticks(rotation=45)
    plt.grid(True, axis='y', linestyle='--', alpha=0.7)
    
    # Add value labels on top of bars
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.2f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('normalized_edp.png')

def create_progress_plots():
    # Read log files
    perf_data = pd.read_csv('ExecutionTime.log', header=None)
    edp_data = pd.read_csv('EnergyEfficiency.log', header=None)
    
    # Plot execution time progress
    plt.figure(figsize=(10, 6))
    plt.plot(range(len(perf_data)), perf_data[1], label='Execution Time')
    plt.title('Normalized Execution Time vs. Designs Evaluated')
    plt.xlabel('Number of Designs Evaluated')
    plt.ylabel('Normalized Execution Time')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('execution_time_progress.png')
    
    # Plot EDP progress
    plt.figure(figsize=(10, 6))
    plt.plot(range(len(edp_data)), edp_data[0], label='EDP')
    plt.title('Normalized Energy-Delay Product vs. Designs Evaluated')
    plt.xlabel('Number of Designs Evaluated')
    plt.ylabel('Normalized EDP')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('edp_progress.png')

# Main execution
def main():
    # Read best configurations
    edp_line, time_line = read_best_file('EnergyEfficiency.best')
    
    # Create all required plots
    create_performance_plot(time_line)
    create_edp_plot(edp_line)
    create_progress_plots()

if __name__ == "__main__":
    main()