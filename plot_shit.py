import os
import re
import matplotlib.pyplot as plt
import numpy as np

def parse_simout_file(filename):
    metrics = {}
    with open(filename, 'r') as f:
        content = f.read()
        
        # Core Performance Metrics
        metrics['ipc'] = float(re.search(r'sim_IPC\s+([\d.]+)', content).group(1))
        metrics['cpi'] = float(re.search(r'sim_CPI\s+([\d.]+)', content).group(1))
        metrics['cycle'] = int(re.search(r'sim_cycle\s+(\d+)', content).group(1))
        metrics['exec_bw'] = float(re.search(r'sim_exec_BW\s+([\d.]+)', content).group(1))
        
        # Branch Prediction Performance
        bp_misses = re.search(r'bpred_[^.]+\.misses\s+(\d+)', content)
        bp_lookups = re.search(r'bpred_[^.]+\.lookups\s+(\d+)', content)
        if bp_misses and bp_lookups:
            metrics['branch_misspred_rate'] = float(bp_misses.group(1)) / float(bp_lookups.group(1))
        
        # Cache Performance - L1 Instruction
        metrics['il1_accesses'] = int(re.search(r'il1.accesses\s+(\d+)', content).group(1))
        metrics['il1_hits'] = int(re.search(r'il1.hits\s+(\d+)', content).group(1))
        metrics['il1_misses'] = int(re.search(r'il1.misses\s+(\d+)', content).group(1))
        metrics['il1_miss_rate'] = float(re.search(r'il1.miss_rate\s+([\d.]+)', content).group(1))
        metrics['il1_hit_rate'] = 1 - metrics['il1_miss_rate']
        
        # Cache Performance - L1 Data
        metrics['dl1_accesses'] = int(re.search(r'dl1.accesses\s+(\d+)', content).group(1))
        metrics['dl1_hits'] = int(re.search(r'dl1.hits\s+(\d+)', content).group(1))
        metrics['dl1_misses'] = int(re.search(r'dl1.misses\s+(\d+)', content).group(1))
        metrics['dl1_miss_rate'] = float(re.search(r'dl1.miss_rate\s+([\d.]+)', content).group(1))
        metrics['dl1_hit_rate'] = 1 - metrics['dl1_miss_rate']
        
        # Cache Performance - L2 Unified
        metrics['ul2_accesses'] = int(re.search(r'ul2.accesses\s+(\d+)', content).group(1))
        metrics['ul2_hits'] = int(re.search(r'ul2.hits\s+(\d+)', content).group(1))
        metrics['ul2_misses'] = int(re.search(r'ul2.misses\s+(\d+)', content).group(1))
        metrics['ul2_miss_rate'] = float(re.search(r'ul2.miss_rate\s+([\d.]+)', content).group(1))
        metrics['ul2_hit_rate'] = 1 - metrics['ul2_miss_rate']
        
        # Pipeline Statistics
        metrics['avg_ruu_occupancy'] = float(re.search(r'ruu_occupancy\s+([\d.]+)', content).group(1))
        metrics['avg_lsq_occupancy'] = float(re.search(r'lsq_occupancy\s+([\d.]+)', content).group(1))
        # Add direct extraction of branch prediction misses
        bp_misses_match = re.search(r'bpred_[^.]+\.misses\s+(\d+)', content)
        if bp_misses_match:
            metrics['branch_misses'] = int(bp_misses_match.group(1))

        return metrics

def plot_separate_graphs():
    data = []
    
    # Read all simout files
    for filename in os.listdir('rawProjectOutputData'):
        if filename.endswith('.simout'):
            config = filename.replace('.simout', '').split('.')
            metrics = parse_simout_file(os.path.join('rawProjectOutputData', filename))
            data.append({'config': config, 'metrics': metrics})
    
    # Sort by evaluation order
    data.sort(key=lambda x: int(x['config'][0]))
    
    # Set default style
    plt.style.use('default')
    plt.rcParams.update({'font.size': 10})

    # 1. Core Performance Plot (IPC)
    plt.figure(figsize=(10, 6))
    ipcs = [d['metrics']['ipc'] for d in data]
    plt.plot(ipcs, marker='o', linewidth=2, markersize=4, color='blue')
    plt.title('Instructions Per Cycle (IPC)', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('IPC')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('ipc_performance.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 2. Branch Prediction Performance
    plt.figure(figsize=(10, 6))
    branch_misspred = [d['metrics']['branch_misspred_rate'] for d in data]
    plt.plot(branch_misspred, marker='s', linewidth=2, markersize=4, color='red')
    plt.title('Branch Misprediction Rate', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Misprediction Rate')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('branch_prediction.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 3. Pipeline Occupancy
    plt.figure(figsize=(10, 6))
    ruu_occ = [d['metrics']['avg_ruu_occupancy'] for d in data]
    lsq_occ = [d['metrics']['avg_lsq_occupancy'] for d in data]
    plt.plot(ruu_occ, label='RUU', marker='v', linewidth=2, markersize=4, color='purple')
    plt.plot(lsq_occ, label='LSQ', marker='^', linewidth=2, markersize=4, color='orange')
    plt.title('Pipeline Queue Occupancy', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Average Occupancy')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('pipeline_occupancy.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 4. L1 Instruction Cache Performance
    plt.figure(figsize=(10, 6))
    il1_hits = [d['metrics']['il1_hit_rate'] for d in data]
    il1_misses = [d['metrics']['il1_miss_rate'] for d in data]
    plt.plot(il1_hits, label='Hit Rate', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(il1_misses, label='Miss Rate', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Instruction Cache Performance', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('l1_icache_performance.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 5. L1 Data Cache Performance
    plt.figure(figsize=(10, 6))
    dl1_hits = [d['metrics']['dl1_hit_rate'] for d in data]
    dl1_misses = [d['metrics']['dl1_miss_rate'] for d in data]
    plt.plot(dl1_hits, label='Hit Rate', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(dl1_misses, label='Miss Rate', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Data Cache Performance', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('l1_dcache_performance.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 6. L2 Cache Performance
    plt.figure(figsize=(10, 6))
    ul2_hits = [d['metrics']['ul2_hit_rate'] for d in data]
    ul2_misses = [d['metrics']['ul2_miss_rate'] for d in data]
    plt.plot(ul2_hits, label='Hit Rate', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(ul2_misses, label='Miss Rate', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L2 Unified Cache Performance', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('l2_cache_performance.png', dpi=300, bbox_inches='tight')
    plt.close()

    # 7. Cache Access Patterns
    plt.figure(figsize=(10, 6))
    accesses_il1 = [d['metrics']['il1_accesses'] for d in data]
    accesses_dl1 = [d['metrics']['dl1_accesses'] for d in data]
    accesses_ul2 = [d['metrics']['ul2_accesses'] for d in data]
    plt.plot(accesses_il1, label='L1-I Accesses', marker='o', linewidth=2, markersize=4, color='blue')
    plt.plot(accesses_dl1, label='L1-D Accesses', marker='s', linewidth=2, markersize=4, color='green')
    plt.plot(accesses_ul2, label='L2 Accesses', marker='^', linewidth=2, markersize=4, color='red')
    plt.title('Cache Access Patterns', fontsize=12, pad=10)
    plt.xlabel('Design Point')
    plt.ylabel('Number of Accesses')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('cache_access_patterns.png', dpi=300, bbox_inches='tight')
    plt.close()
    plot_hit_miss_trends(data)

def plot_hit_miss_trends(data):
    plt.figure(figsize=(12, 8))
    
    # Branch Prediction Misses
    bpred_misses = [int(d['metrics'].get('branch_misspred_rate', 0) * 
                       50000) for d in data]  # Assuming 50000 total branch predictions
    
    plt.subplot(2, 2, 1)
    plt.plot(bpred_misses, label='Branch Misses', color='red', marker='o', linewidth=2, markersize=4)
    plt.title('Branch Prediction Misses Over Time', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Number of Misses')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()

    # Cache Hits and Misses
    plt.subplot(2, 2, 2)
    il1_hits = [d['metrics']['il1_hits'] for d in data]
    il1_misses = [d['metrics']['il1_misses'] for d in data]
    plt.plot(il1_hits, label='L1-I Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(il1_misses, label='L1-I Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Instruction Cache Hits/Misses', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()

    plt.subplot(2, 2, 3)
    dl1_hits = [d['metrics']['dl1_hits'] for d in data]
    dl1_misses = [d['metrics']['dl1_misses'] for d in data]
    plt.plot(dl1_hits, label='L1-D Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(dl1_misses, label='L1-D Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Data Cache Hits/Misses', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()

    plt.subplot(2, 2, 4)
    ul2_hits = [d['metrics']['ul2_hits'] for d in data]
    ul2_misses = [d['metrics']['ul2_misses'] for d in data]
    plt.plot(ul2_hits, label='L2 Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(ul2_misses, label='L2 Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L2 Cache Hits/Misses', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()

    plt.tight_layout()
    plt.savefig('hit_miss_trends.png', dpi=300, bbox_inches='tight')
    plt.close()

    # Create individual plots for each trend
    # Branch Prediction Misses
    plt.figure(figsize=(10, 6))
    plt.plot(bpred_misses, label='Branch Misses', color='red', marker='o', linewidth=2, markersize=4)
    plt.title('Branch Prediction Misses Over Time', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Number of Misses')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig('branch_misses_trend.png', dpi=300, bbox_inches='tight')
    plt.close()

    # L1 Instruction Cache
    plt.figure(figsize=(10, 6))
    plt.plot(il1_hits, label='Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(il1_misses, label='Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Instruction Cache Hits/Misses Over Time', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig('l1_icache_hits_misses.png', dpi=300, bbox_inches='tight')
    plt.close()

    # L1 Data Cache
    plt.figure(figsize=(10, 6))
    plt.plot(dl1_hits, label='Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(dl1_misses, label='Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L1 Data Cache Hits/Misses Over Time', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig('l1_dcache_hits_misses.png', dpi=300, bbox_inches='tight')
    plt.close()

    # L2 Cache
    plt.figure(figsize=(10, 6))
    plt.plot(ul2_hits, label='Hits', color='green', marker='o', linewidth=2, markersize=4)
    plt.plot(ul2_misses, label='Misses', color='red', marker='x', linewidth=2, markersize=4)
    plt.title('L2 Cache Hits/Misses Over Time', fontsize=12)
    plt.xlabel('Design Point')
    plt.ylabel('Count')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig('l2_cache_hits_misses.png', dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    plot_separate_graphs()