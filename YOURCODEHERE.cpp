 #include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (963525236+000000000)///-->No partner

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 *
 * Can definetly clean this up, or maybe not.

 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;
// Define dimension groups based on PSU ID sum (FPU → Cache → BP → Core)

// Global tracking variables
unsigned int currentGroupIndex = 0;
unsigned int currentDimIndexInGroup = 0;
std::vector<std::vector<int>> dimensionGroups;
bool isCurrentDimDone = false;

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
/**
 * The Latency Chart:
				The il1 sizes and il1 latencies are linked as follows (the same linkages hold for the dl1 size and dl1 latency):
					(a) il1 = 2 KB means il1lat = 1
					(b) il1 = 4 KB means il1lat = 2
					(c) il1 = 8 KB means il1lat = 3
					(d) il1 = 16 KB means il1lat = 4
					(e) il1 = 32 KB means il1lat = 5
					(f) il1 = 64 KB means il1lat = 6
					(g) The above are for direct mapped caches. For 2-way set associative add 1
					additional cycle of latency to each of the above; for 4-way add 2 additional
					cycles; for 8-way add 3 additional cycles.
					6. The ul2 sizes and ul2 latencies are linked as follows:
					(a) ul2 = 32 KB means ul2lat = 5
					(b) ul2 = 64 KB means ul2lat = 6
					7
					(c) ul2 = 128 KB means ul2lat = 7
					(d) ul2 = 256 KB means ul2lat = 8
					(e) ul2 = 512 KB means ul2 lat = 9
					(f) ul2 = 1024 KB (1 MB) means ul2lat = 10	
 */
///Dimension tracking param...
enum DimensionGroup { FPU_GROUP, CACHE_GROUP, BP_GROUP, CORE_GROUP };
DimensionGroup currentGroup = FPU_GROUP;

std::string generateCacheLatencyParams(string halfBackedConfig) {
/** 
	string latencySettings;
	//
	//YOUR CODE BEGINS HERE
	//
	int L1_Block = 

	// This is a dumb implementation.
	latencySettings = "1 1 1";

	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings;
	*/
	/**
	 * I'm going to take a sort of half baked approach...
	 * 
	 * general approach: follow the guide :/ 
	 */
	// String stream to build our final latency string
std::stringstream latencysettings;

// Calculate base cache parameters
// L1 caches
int L1_Block = 8;  // Start with 8 byte blocks
L1_Block = L1_Block * (1 << extractConfigPararm(halfBackedConfig, 2));

// Data L1 cache
int dL1_Sets = 32;  // Base number of sets
dL1_Sets = dL1_Sets << extractConfigPararm(halfBackedConfig, 3);
int dL1_Associative = 1;  // Start with direct mapped
dL1_Associative = dL1_Associative << extractConfigPararm(halfBackedConfig, 4);

// Instruction L1 cache
int iL1_Sets = 32;  // Base number of sets
iL1_Sets = iL1_Sets << extractConfigPararm(halfBackedConfig, 5);
int iL1_Associative = 1;  // Start with direct mapped
iL1_Associative = iL1_Associative << extractConfigPararm(halfBackedConfig, 6);

// L2 unified cache
int uL2_Sets = 256;  // More sets than L1
uL2_Sets = uL2_Sets << extractConfigPararm(halfBackedConfig, 7);
int uL2_Block = 16;  // Bigger blocks than L1
uL2_Block = uL2_Block << extractConfigPararm(halfBackedConfig, 8);
int uL2_Associative = 1;
uL2_Associative = uL2_Associative << extractConfigPararm(halfBackedConfig, 9);

// Figure out total cache sizes
int dL1_Size = dL1_Sets * dL1_Associative * L1_Block;  // Data L1 total size
int iL1_Size = iL1_Sets * iL1_Associative * L1_Block;  // Instruction L1 total size

// Start with minimum latencies for L1 caches
int dL1_Latency = 1;
int iL1_Latency = 1;

// Calculate Data L1 latency based on size
// Bigger caches take longer to access
if (dL1_Size <= 2048) {        // 2KB
    dL1_Latency = 1;
} else if (dL1_Size <= 4096) { // 4KB
    dL1_Latency = 2;
} else if (dL1_Size <= 8192) { // 8KB
    dL1_Latency = 3;
} else if (dL1_Size <= 16384) { // 16KB
    dL1_Latency = 4;
} else if (dL1_Size <= 32768) { // 32KB
    dL1_Latency = 5;
} else if (dL1_Size <= 65536) { // 64KB
    dL1_Latency = 6;
}

// Do the same for Instruction L1 latency
if (iL1_Size <= 2048) {        // 2KB
    iL1_Latency = 1;
} else if (iL1_Size <= 4096) { // 4KB
    iL1_Latency = 2;
} else if (iL1_Size <= 8192) { // 8KB
    iL1_Latency = 3;
} else if (iL1_Size <= 16384) { // 16KB
    iL1_Latency = 4;
} else if (iL1_Size <= 32768) { // 32KB
    iL1_Latency = 5;
} else if (iL1_Size <= 65536) { // 64KB
    iL1_Latency = 6;
}

// Add extra cycles for higher associativity
// More ways = more tag comparisons = more time
if (dL1_Associative == 2) {
    dL1_Latency += 1;  // 2-way takes 1 extra cycle
} else if (dL1_Associative == 4) {
    dL1_Latency += 2;  // 4-way takes 2 extra cycles
} else if (dL1_Associative == 8) {
    dL1_Latency += 3;  // 8-way takes 3 extra cycles
}

// Same associativity penalties for instruction cache
if (iL1_Associative == 2) {
    iL1_Latency += 1;
} else if (iL1_Associative == 4) {
    iL1_Latency += 2;
} else if (iL1_Associative == 8) {
    iL1_Latency += 3;
}

// Calculate L2 cache size and base latency
int ul2_size = uL2_Sets * uL2_Associative * uL2_Block;
int ul2_Latency = 5;  // Start with minimum L2 latency

// L2 latency increases with size
if (ul2_size <= 32768) {         // 32KB
    ul2_Latency = 5;
} else if (ul2_size <= 65536) {  // 64KB
    ul2_Latency = 6;
} else if (ul2_size <= 131072) { // 128KB
    ul2_Latency = 7;
} else if (ul2_size <= 262144) { // 256KB
    ul2_Latency = 8;
} else if (ul2_size <= 524288) { // 512KB
    ul2_Latency = 9;
} else if (ul2_size <= 1048576) { // 1MB
    ul2_Latency = 10;
}

// Add L2 associativity penalty
if (uL2_Associative == 2) {
    ul2_Latency += 1;       // 2-way
} else if (uL2_Associative == 4) {
    ul2_Latency += 2;       // 4-way
} else if (uL2_Associative == 8) {
    ul2_Latency += 3;       // 8-way
} else if (uL2_Associative == 16) {
    ul2_Latency += 4;       // 16-way
}

// Debug prints (commented out)
/*
printf("Data L1: size=%d bytes, assoc=%d-way, latency=%d cycles\n", 
       dL1_Size, dL1_Associative, dL1_Latency);
printf("Inst L1: size=%d bytes, assoc=%d-way, latency=%d cycles\n", 
       iL1_Size, iL1_Associative, iL1_Latency);
printf("L2: size=%d bytes, assoc=%d-way, latency=%d cycles\n",
       ul2_size, uL2_Associative, ul2_Latency);
*/


latencysettings << dL1_Latency << " " << iL1_Latency << " " << ul2_Latency;
return latencysettings.str();
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {

	// FIXME - YOUR CODE HERE
	/**
	 * I'm going to take a sort of half baked approach...
	 * 
	 * general approach:
	 * 1. Check if the current config is valid using already approached return value of
	 * 	isNumDimConfiguration(configuration) --> help check if valid
	 * 2. If the return is true then, we can do the following usign the 8.3 items:
	 * 	a. Extract L1 & L2 cache using the helper function of below, being aware of the latency: 
	 * 		int extractConfigPararm(std::string config, int paramIndex);
	 * 	b. Validate that the L1 cache block size is " for the baseline machine the ifqsize is set to 1 word (8B) then
														the il1 block size should be at least 8B""
	 *  c. Validate that L2 cache block size is" ul2 (unified L2 cache) block size must be at least twice your il1 (and dl1)
													block size with a maximum block size of 128B"
 	 * 	
	 */
	//check if valid config with helper
	if(!isNumDimConfiguration(configuration))
	{
		return(0);
	}
	///	IS THERE A WAY TO MAKE THIS A GLOBLA PARAMETER FOR OTHER USE-CASES???? 
// Get L1 cache block size (in bytes)
int L1_Block = 8;  // Start with 8 bytes
L1_Block = L1_Block * (1 << extractConfigPararm(configuration, 2));  // Scale up by power of 2

// Get L1 Data cache parameters
int dL1_Set = 32;  // Base number of sets
dL1_Set = dL1_Set << extractConfigPararm(configuration, 3);  // Scale sets by power of 2
int dL1_Associative = 1;  // Start with 1 way
dL1_Associative = dL1_Associative << extractConfigPararm(configuration, 4);  // Get final associativity

// Get L1 Instruction cache parameters
int iL1_Sets = 32;  // Base number of sets
iL1_Sets = iL1_Sets << extractConfigPararm(configuration, 5);  // Scale sets by power of 2
int iL1_Associative = 1;  // Start with 1 way
iL1_Associative = iL1_Associative << extractConfigPararm(configuration, 6);  // Get final associativity

// Get L2 unified cache parameters
int uL2_Block = 16;  // L2 starts with bigger blocks
uL2_Block = uL2_Block << extractConfigPararm(configuration, 8);  // Scale block size
int uL2_Sets = 256;  // L2 has more sets than L1
uL2_Sets = uL2_Sets << extractConfigPararm(configuration, 7);  // Scale number of sets
int uL2_Associative = 1;  // Start with 1 way
uL2_Associative = uL2_Associative << extractConfigPararm(configuration, 9);  // Get final associativity

// Calculate total cache sizes
int dL1_Size = dL1_Set * dL1_Associative * L1_Block;  // Total data L1 cache size
int iL1_Size = iL1_Sets * iL1_Associative * L1_Block;  // Total instruction L1 cache size
int uL2_Size = uL2_Sets * uL2_Associative * uL2_Block;  // Total L2 cache size
    /*
    printf("L1 block size: %d bytes\n", L1_Block);
    printf("Data L1: sets=%d, ways=%d, total=%d bytes\n", 
        dL1_Set, dL1_Associative, dL1_Size);
    printf("Inst L1: sets=%d, ways=%d, total=%d bytes\n", 
        iL1_Sets, iL1_Associative, iL1_Size);
    printf("L2: sets=%d, ways=%d, block=%d, total=%d bytes\n",
        uL2_Sets, uL2_Associative, uL2_Block, uL2_Size);


        The il1 (L1 instruction cache) block size must be at least the ifq (instruction fetch
		queue) size (e.g., for the baseline machine the ifqsize is set to 1 word (8B) then
		the il1 block size should be at least 8B)
    */

	if(L1_Block < 8)
	{
		return(0);
	}
	/**
	 *  The ul2 (unified L2 cache) block size must be at least twice your il1 (and dl1)
		block size with a maximum block size of 128B. 
	 */
	if((uL2_Block < 2*L1_Block) || (uL2_Block > 128))
	{
		return(0);
	}
	/**
	 * il1 size and dl1 size: Minimum = 2 KB ~2048 Bytes; Maximum = 64 KB ~65536 Bytes
	 */
	if((dL1_Size < 2048) || (dL1_Size > 65536) || (iL1_Size < 2048) || (iL1_Size > 65536))
	{
		return(0);
	}
	/**
	 * ul2 size: Minimum = 32 KB ~ 32768 Bytes; Maximum = 1 MB ~ 1048576 Bytes
	 */
	if((uL2_Size < 32768)|| (uL2_Size > 1048576))
	{
		return(0);
	}
	/**
	 * Your ul2 must be at least twice as large as il1+dl1 in order to be inclusive
	 */
	if(uL2_Size < (2*(dL1_Size+iL1_Size)))
	{
		return(0);
	}
	//end, return valid
	return(1);

	/**
	 * // The below is a necessary, but insufficient condition for validating a
	   // configuration.
	   return isNumDimConfiguration(configuration);
	 */
	
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. 
 * 
 * You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 * 
 * YOU WILL NEED TO CHANGE THIS AND MAKE THIS MORE EFFICIENT FOLLOWING THE SEQUENCE 
 * WE ARE GIVEN FOR ME IT IS Following the format of FPU --> Cache --> BP--> Core 
 *  
 * 
 * the following implimentation guidline that deviates from the dum approach :\
 * 1. Tracking of the best config & metrics for each dimension group (all ~18 groups)
 * 2. global tracking variables
 * 
 * Deviations from Dumb approach:
 * 1. First determine the current dimension group and the necessary param
 * 2. Move following the FPU-->Cache-->BP-->Core schema, 
 * 	  2.1). sizing of l1 & L2 cache consitency is needed****REFERENCE ISTRUCTION SHEET
 * 3. Explore each parameter concurrently in terms of relationship management
 * 	*basically param relate to one another where for example the cache block size can be tied with assocaitivity & etc
 * 4. Documentation of decision making (a decision tree implim can be done later)
 * 	i.e. log exploration decision, record metrics
 * 5. maintain config consistency within groups :/ 
 * *I think that's all that is being asked so far... 
 * */// Essential state tracking
// FPU->Cache->BP->Core for PSU ID mod 24 = 20// Global constants for dimension groups - keep it simple with descriptive names
const vector<vector<int>> DIMENSION_GROUPS = {
    {11},                         // FPU stuff
    {2,3,4,5,6,7,8,9},           // Cache params 
    {12,13,14},                  // Branch pred
    {0,1}                       // Core configs
};

// Basic struct to keep track of where we are in exploration
struct ExplorationState {
    unsigned int currGroupIdx;  
    unsigned int currDimInGroup;
    double bestMetricSoFar;
    unsigned int numEvals;
    
    // Store best configs we've found for each group
    struct GroupInfo {
        double bestMetric;
        vector<int> bestParams; 
        bool gotBetter;
    };
    map<int, GroupInfo> groupResults;
    
    // Constructor with some sane defaults
    ExplorationState() {
        currGroupIdx = 0;
        currDimInGroup = 0; 
        bestMetricSoFar = 999999999.9;  // Really big number
        numEvals = 0;
    }
};

// Global state - not great practice but keeps things simple
static ExplorationState exploreState;

// Helper function to test a configuration and get its performance
bool tryConfiguration(const string& config, bool isEDP, double& result) {
    // Make sure we haven't tested this before
    if(GLOB_seen_configurations.find(config) == GLOB_seen_configurations.end()) {
        if(runexperiments(config, 0) != 0) {
            return false; // Something went wrong
        }
        populate(config);
    }

    // Get the results if they exist
    if(!GLOB_extracted_values[config]) {
        return false;
    }

    // Calculate either EDP or execution time
    if(isEDP) {
        result = calculategeomeanEDP(config);
    } else {
        result = calculategeomeanExecutionTime(config);
    }
    return true;
}

// Check if a configuration makes sense based on some basic rules
bool configLooksGood(const vector<int>& params, int dim) {
    // FPU width shouldn't be bigger than pipeline width
    if(dim == 11) {
        int pipeWidth = 1 << params[0];
        int fpWidth = 1 << params[11];
        if(fpWidth > pipeWidth) {
            return false;
        }
    }
    
    // Cache stuff
    if(dim >= 2 && dim <= 9) {
        // L1 blocks should be reasonable size
        if(dim == 2) {
            int blockSize = 8 * (1 << params[2]);
            if(blockSize < 32 || blockSize > 64) {
                return false;
            }
        }
        
        // L1 caches like lower associativity
        if(dim == 4 || dim == 6) {
            if(params[dim] > 1) {
                return false;
            }
        }
        
        // L2 needs more associativity
        if(dim == 9 && params[9] < 2) {
            return false;
        }
    }
    
    // Branch predictor settings
    if(dim >= 12 && dim <= 14) {
        // Tournament predictor works better
        if(dim == 12 && params[12] != 4) {
            return false;
        }
        // Need decent RAS size
        if(dim == 13 && params[13] < 1) {
            return false;
        }
    }
    
    // Core config - wide in-order probably bad
    if(dim <= 1) {
        if(dim == 0 && params[0] >= 2 && params[1] == 0) {
            return false;
        }
    }
    
    return true;
}

// Main function to propose next configuration to test
string generateNextConfigurationProposal(string currConfig, 
                                       string bestExecConfig, 
                                       string bestEDPConfig,
                                       int doExec, 
                                       int doEDP) {
    
    // Don't go over our eval limit
    if(exploreState.numEvals >= 1000) {
        return doEDP ? bestEDPConfig : bestExecConfig;
    }
    
    try {
        // Parse current config into vector
        vector<int> params;
        stringstream ss(currConfig);
        int val;
        while(ss >> val) {
            params.push_back(val);
        }

        if(params.size() != NUM_DIMS) {
            return currConfig; // Something's wrong
        }
        
        if(exploreState.currGroupIdx >= DIMENSION_GROUPS.size()) {
            return currConfig; // We're done
        }
        
        // Try variations of current dimension
        int currDim = DIMENSION_GROUPS[exploreState.currGroupIdx][exploreState.currDimInGroup];
        bool foundGood = false;
        int tries = 0;
        while(!foundGood && tries < GLOB_dimensioncardinality[currDim]*2 && 
              exploreState.numEvals < 1000) {
            
            // Try next value for this parameter
            params[currDim]++;
            if(params[currDim] >= GLOB_dimensioncardinality[currDim]) {
                params[currDim] = 0;
            }
            
            // Skip if it looks bad
            if(!configLooksGood(params, currDim)) {
                tries++;
                continue;
            }
            
            // Build config string
            stringstream newConfig;
            for(size_t i = 0; i < params.size(); i++) {
                newConfig << params[i];
                if(i < params.size()-1) newConfig << " ";
            }
            string configStr = newConfig.str();
            
            // Skip if invalid or already seen
            if(!validateConfiguration(configStr) || 
               GLOB_seen_configurations.find(configStr) != GLOB_seen_configurations.end()) {
                tries++;
                continue; 
            }

            // Try it out
            double metric;
            if(!tryConfiguration(configStr, doEDP==1, metric)) {
                tries++;
                continue;
            }

            exploreState.numEvals++;
            
            // Update best results for this group
            auto& groupInfo = exploreState.groupResults[exploreState.currGroupIdx];
            if(metric < groupInfo.bestMetric) {
                groupInfo.bestMetric = metric;
                groupInfo.bestParams = params;
                groupInfo.gotBetter = true;
            }

            if(metric < exploreState.bestMetricSoFar) {
                exploreState.bestMetricSoFar = metric;
            }

            foundGood = true;
            GLOB_seen_configurations[configStr] = 1;
            return configStr;
        }
        
        // If dimension isn't helping, use best known values
        auto& groupInfo = exploreState.groupResults[exploreState.currGroupIdx];
        if(!groupInfo.gotBetter && !groupInfo.bestParams.empty()) {
            for(int dim : DIMENSION_GROUPS[exploreState.currGroupIdx]) {
                params[dim] = groupInfo.bestParams[dim];
            }
        }
        
        // Move to next dimension/group
        exploreState.currDimInGroup++;
        if(exploreState.currDimInGroup >= DIMENSION_GROUPS[exploreState.currGroupIdx].size()) {
            exploreState.currGroupIdx++;
            exploreState.currDimInGroup = 0;
        }
        
        if(exploreState.currGroupIdx >= DIMENSION_GROUPS.size()) {
            return doEDP ? bestEDPConfig : bestExecConfig;
        }
        
        // Try next config
        return generateNextConfigurationProposal(currConfig, bestExecConfig, 
                                               bestEDPConfig, doExec, doEDP);
        
    } catch(...) {
        return currConfig; // If anything goes wrong, return original
    }
}