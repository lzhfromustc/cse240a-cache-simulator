//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"

//
// TODO:Student Information
//
const char *studentName = "Ziheng Liu";
const char *studentID   = "A59010078";
const char *email       = "zil060@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

// Utilities
#define DEBUG

// L1 I-cache

typedef struct icache_block_struct {
  uint32_t tag;
  uint8_t valid;
  uint32_t lru; // when a block is used, set its lru to 0, and all other blocks in the same set have their lru ++.
                // TODO: when a block's lru is too large, decrease lru
} icache_block;

typedef struct icache_set_struct {
  icache_block *blocks;
} icache_set;

typedef struct icache_struct {
  icache_set *sets;
} icache;

icache icacheObj;


//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Utilities
uint32_t my_pow2(uint32_t input) {
  return 1 << input;
}

uint32_t my_log2(uint32_t input) {
  for (uint32_t i = 0; i < 100; i++) {
    uint32_t try = 1 << i;
    if (try == input) {
      return i;
    }
  }
  return 0;
}

void test_my_util() {
  if (my_log2(2048) != 11) {
    printf("\tError in test util: my_log2(2048) == %u\n", my_log2(2048));
  }
  if (my_pow2(13) != 8192) {
    printf("\tError in test util: my_pow2(13) == %u\n", my_pow2(13));
  }
}

// Initialize the Cache Hierarchy
//
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;
  
  //
  //TODO: Initialize Cache Simulator Data Structures
  //

  // test my utilities
  #ifdef DEBUG
  test_my_util();
  #endif // DEBUG

  // if I-cache exists, initialize it
  if (icacheSets) {
    icacheObj.sets = (icache_set*)malloc(icacheSets * sizeof(icache_set));
    for (int i = 0; i < icacheSets; i++) {
      icacheObj.sets[i].blocks = (icache_block*)malloc(icacheAssoc * sizeof(icache_block));
    }
    #ifdef DEBUG
    printf("\tLog: Init state: I-Cache is initialized\n");
    #endif
  }
  
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  //
  //TODO: Implement I$
  //
  return memspeed;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  //
  //TODO: Implement D$
  //
  return memspeed;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  //
  //TODO: Implement L2$
  //
  return memspeed;
}
